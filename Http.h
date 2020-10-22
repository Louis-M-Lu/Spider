#ifndef _HTTP_H
#define _HTTP_H
#include <iostream>

// ��ֹͷ�ļ��ظ�����

class CHttp
{
public:
	std::string m_host;						//����
	std::string m_object;					//��Դ·��
	SOCKET		m_socket;					//client�׽���

public:
	bool AnalyseUrl(std::string url);		//����url
	bool InitSock();						//��ʼ������
	bool Connect();							//��������
	bool GetHtml(std::string &html);		//��ȡ��������
	bool AnalyseHtml(std::string html);		//������ҳ
	bool Download(std::string url, std::string filename);	//�����ļ�
	bool Close();							//�ر��׼���
};

#endif