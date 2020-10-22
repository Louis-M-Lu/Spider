#include <iostream>
#include <string>
#include <WinSock2.h>
#include <queue>
#include "Http.h"
#pragma comment(lib, "ws2_32.lib")

//�ж�����Э��
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
	//�ַ�����ȡ���ָ�

	if (url.empty())
		return false;

	if (url.length() <= 7)	//	����http://
		return false;
 
	if (url.find("http://") == std::string::npos)	//npos:������
		return false;
		

	int pos = url.find('/', 7);	//�ӵ�7����ʼ��:http://www.abc.com/	����'w'��ʼ��
	if (pos == std::string::npos)	//http://www.abc.com
	{
		m_host		= url.substr(7);
		m_object	= "/";
	}
	else	//http://www.abc.com/def/ghi...
	{
		m_host		= url.substr(7, pos - 7);	//��ȡwww.abc.com
		m_object	= url.substr(pos);			//�ӡ�/����ʼ��
	}

	if (m_host.empty())
		return false;

	std::cout << "����:" << m_host << "\t" << "��Դ" << m_object << std::endl;

	return true;
}

//����URL\https
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

//��ʼ������
bool CHttp::InitSock()
{
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2,2), &wd) != 0)
		return false;

	//�жϼ��ع������׽��ֿ�汾�Ƿ�һ��
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)
		return false;
	
	//�����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		return false;

	return true;
}

//��������
bool CHttp::Connect()
{
	//��ʼ������
	if (InitSock() == false)
		return false;

	//����IP��ַ
	hostent* host = gethostbyname(m_host.c_str());
	if (host == NULL)
		return false;

	//����Web������
	sockaddr_in sa;
	sa.sin_family	= AF_INET;
	sa.sin_port		= htons(80);
	memcpy(&sa.sin_addr, host->h_addr, 4);

	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&sa, sizeof(sa)))
		return false;


	return true;
}

//�ر��׼���
bool CHttp::Close()
{

	closesocket(m_socket);
	m_host.empty();
	m_object.empty();
	m_socket = 0;

	return true;
}

//��ȡ��������
bool CHttp::GetHtml(std::string &html)
{
	//����GET����
	std::string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n";
	info = info + "User-Agent: Microsoft-CryptoAPI/10.0\r\n";
	info = info + "\r\n";

	if (send(m_socket, info.c_str(), info.length(), 0) == SOCKET_ERROR)
		return false;

	//��������
	char ch = 0;
	while (recv(m_socket, &ch, sizeof(ch), 0))
	{
		html += ch;
	}

	return true;
}

//�����ļ�
bool CHttp::Download(std::string url, std::string filename)
{
	  
	if (false == AnalyseUrl(url))
		return false;

	if (false == Connect())
		return false;


	//����GET����
	std::string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "Connection: Close\r\n";
	info = info + "User-Agent: Microsoft-CryptoAPI/10.0\r\n";
	info = info + "\r\n";

	if (send(m_socket, info.c_str(), info.length(), 0) == SOCKET_ERROR)
		return false;

	//���ļ�
	FILE* fp = fopen(filename.c_str(), "wb");
	if (fp == NULL)
		return false;

	//����ͷ��Ϣ
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

	//��������
	while (recv(m_socket, &ch, sizeof(ch), 0))
	{
		fwrite(&ch, 1, 1, fp);
	}

	//�ر��ļ�
	fclose(fp);

	return true;
}