#include <iostream>
#include <string>
#include <WinSock2.h>
#include <queue>
#include "Http.h"
#pragma comment(lib, "ws2_32.lib")

//判断网络协议
/*int CHttp::JudgeAgreement(std::string url)
{
	if (url.empty())
		return -1;

	if (url.length() <= 7)
		return -1;

	if (url.find("http://") != std::string::npos)
		return 2;

	if (url.find("https://") != std::string::npos)
		return 3;

	return 0;
}
*/

bool CHttp::AnalyseUrl(std::string url)
{
	//字符串截取、分割

	if (url.empty())
		return false;

	if (url.length() <= 7)	//	不是http://
		return false;
 
	if (url.find("http://") == std::string::npos)	//npos:表不存在
		return false;
		

	int pos = url.find('/', 7);	//从第7个开始找:http://www.abc.com/	即从'w'开始找
	if (pos == std::string::npos)	//http://www.abc.com
	{
		m_host		= url.substr(7);
		m_object	= "/";
	}
	else	//http://www.abc.com/def/ghi...
	{
		m_host		= url.substr(7, pos - 7);	//截取www.abc.com
		m_object	= url.substr(pos);			//从‘/’后开始找
	}

	if (m_host.empty())
		return false;

	std::cout << "域名:" << m_host << "\t" << "资源" << m_object << std::endl;

	return true;
}

//解析URL\https
/*bool CHttp::AnalyseUrlHttps(std::string url)
{
	if (std::string::npos == url.find("https://"))
		return false;

	if (url.length() <= 8)
		return false;

	int pos = url.find('/', 8);
	if (pos == std::string::npos)
	{
		m_host	  = url.substr(8);
		m_object  = '/';
	}
	else
	{
		m_host    = url.substr(8, pos - 8);
		m_object  = url.substr(pos);
	}
	if (m_host.empty())
		return false;

	return true;
}
*/

//初始化网络
bool CHttp::InitSock()
{
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2,2), &wd) != 0)
		return false;

	//判断加载过来的套接字库版本是否一致
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
		return false;
	
	//创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		return false;

	return true;
}

//连接网络
bool CHttp::Connect()
{
	//初始化网络
	if (InitSock() == false)
		return false;

	//解析IP地址
	hostent* host = gethostbyname(m_host.c_str());
	if (host == NULL)
		return false;

	//连接Web服务器
	sockaddr_in sa;
	sa.sin_family	= AF_INET;
	sa.sin_port		= htons(80);
	memcpy(&sa.sin_addr, host->h_addr, 4);

	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&sa, sizeof(sa)))
		return false;


	return true;
}

//关闭套件字
bool CHttp::Close()
{

	closesocket(m_socket);
	m_host.empty();
	m_object.empty();
	m_socket = 0;

	return true;
}

//获取网络内容
bool CHttp::GetHtml(std::string &html)
{
	//发送GET请求
	std::string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n";
	info = info + "User-Agent: Microsoft-CryptoAPI/10.0\r\n";
	info = info + "\r\n";

	if (send(m_socket, info.c_str(), info.length(), 0) == SOCKET_ERROR)
		return false;

	//接收数据
	char ch = 0;
	while (recv(m_socket, &ch, sizeof(ch), 0))
	{
		html += ch;
	}

	return true;
}

//下载文件
bool CHttp::Download(std::string url, std::string filename)
{
	  
	if (false == AnalyseUrl(url))
		return false;

	if (false == Connect())
		return false;


	//发送GET请求
	std::string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n";
	info = info + "User-Agent: Microsoft-CryptoAPI/10.0\r\n";
	info = info + "\r\n";

	if (send(m_socket, info.c_str(), info.length(), 0) == SOCKET_ERROR)
		return false;

	//打开文件
	FILE* fp = fopen(filename.c_str(), "wb");
	if (fp == NULL)
		return false;

	//接收头信息
	char ch = 0;	//\r\n\r\n
	while (recv(m_socket, &ch, 1, 0))
	{
		if (ch == '\r')
		{
			recv(m_socket, &ch, 1, 0);
			if (ch == '\n')
			{
				recv(m_socket, &ch, 1, 0);
				if (ch == '\r')
				{
					recv(m_socket, &ch, 1, 0);
					if (ch == '\n')
					{
						break;
					}
				}
			}
		}
	}

	//接收数据
	while (recv(m_socket, &ch, sizeof(ch), 0))
	{
		fwrite(&ch, 1, 1, fp);
	}

	//关闭文件
	fclose(fp);

	return true;
}