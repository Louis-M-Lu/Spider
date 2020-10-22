#include <iostream>
#include <Windows.h>
#include <string>
#include <queue>	//队列
#include <regex>	//正则表达式
#include "Http.h"

using namespace std;

string url;
string path;
queue <string> q;	//Url队列

bool StartCatch(string url);

int main()
{
	cout << "====================================" << endl;
	cout << "	  网络爬虫系统 " << endl;
	cout << "	   版本: v1.0 " << endl;
	cout << "Made by luzimo  from Codeing tang " << endl;
	cout << "====================================" << endl;


	cout << "请输入需要抓取的网址：";
	cin >> url;

	cout << "请输入保存路径及文件夹名(如D:\\test, test为要创建建的文件夹名，不能重复)：";
	cin >> path;

	//创建文件夹
	CreateDirectory(path.c_str(), NULL);

	//开抓

	StartCatch(url);
	
	system("pause");
	return 0;
}

bool StartCatch(string url)
{

	q.push(url);

	while (!q.empty())
	{
		//取出一条url
		string cur = q.front();
		q.pop();
		
		CHttp http;
		//网络解析
		http.AnalyseUrl(cur);

		if (http.Connect() == false)
		{
			cout << "连接失败！" << endl;
		}
		else
		{
			cout << "连接成功！" << endl;
		}

		string html;
		http.GetHtml(html);
		http.Close();

		vector <string> vecImage;

		//解析网页内容
		smatch mat;
		regex rex("http://[^\\s'\"<>()]+");	//正则表达式，匹配要有“http://”一次(+表示)，不能有
											//[]内的东西（^表取反）
		string::const_iterator start = html.begin();
		string::const_iterator end	 = html.end();

		while (regex_search(start, end, mat, rex))
		{
			string per(mat[0].first, mat[0].second);
			
			cout << per << endl;

			//匹配到了图片
			if (per.find(".jpg") != string::npos || per.find(".png") != string::npos ||
				per.find(".jpeg") != string::npos)
			{
				vecImage.push_back(per);
			}
			else
			{
				//去除 w3c
				if (per.find("http://www.w3.org/") == string::npos)
					q.push(per);
			}

			start = mat[0].second;
		}

		//下载图片
		for (int i = 0; i < vecImage.size(); i++)
		{
			string filename = path + '\\' +	
				vecImage[i].substr(vecImage[i].find_last_of('/') + 1);
			CHttp httpDownload;

			if (false == httpDownload.Download(vecImage[i], filename))
			{
				cout << "下载失败" << GetLastError() << endl;
			}
			else
			{
				cout << "下载成功" << endl;
			}
		}

	}

	return true;
}
