#ifndef _HTTP_H
#define _HTTP_H
#include <iostream>

// 防止头文件重复包含

class CHttp
{
public:
	std::string m_host;						//域名
	std::string m_object;					//资源路径
	SOCKET		m_socket;					//client套接字

public:
	bool AnalyseUrl(std::string url);		//解析url
	bool InitSock();						//初始化网络
	bool Connect();							//连接网络
	bool GetHtml(std::string &html);		//获取网络内容
	bool AnalyseHtml(std::string html);		//解析网页
	bool Download(std::string url, std::string filename);	//下载文件
	bool Close();							//关闭套件字
};

#endif