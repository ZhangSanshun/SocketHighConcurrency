/**
* @file		Client.cpp
* @brief
* @copyright	Copyright (c) 2020 ZhangSS
* @par 修改记录:
* <table>
* <tr><th>修改日期    <th>修改人    <th>描述
* <tr><td>2020-12-28  <td>ZhangSS  <td>创建第一版
* <tr><td>2021-03-05  <td>ZhangSS  <td>支持Linux系统
* </table>
*/

#include "EasyTcpClient.hpp"

bool g_bRun = true;

void CmdTread()
{
	while (true)
	{
		char cmdBuf[1024] = {};
		scanf("%s", cmdBuf);

		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			//client->Close();
			printf("Exit thread!!!\n");
			break;
		}
// 		else if (0 == strcmp(cmdBuf, "login"))
// 		{
// 			LoginData login;
// 			strcpy(login.userName, "pql");
// 			strcpy(login.passWord, "pql1115");
// 			client->SendData(&login);
// 		}
// 		else if (0 == strcmp(cmdBuf, "logout"))
// 		{
// 			LogoutData logout;
// 			strcpy(logout.userName, "pql");
// 
// 			client->SendData(&logout);
// 		}
// 		else
// 		{
// 			printf("input data error!!!\n");
// 		}
	}

}

//客户端数量
const int cCount = 1000;
//线程数量
const int tCount = 4;

//客户端线程
EasyTcpClient* client[cCount];

void SendThread(int id)
{

	int begin = (id - 1) * (cCount / tCount);
	int end = id * (cCount / tCount);

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i] = new EasyTcpClient();
	}

	for (int i = begin; i < end; i++)
	{
		if (!g_bRun)
		{
			return;
		}
		client[i]->Connect("127.0.0.1", 4567);
	}

	LoginData login;
	strcpy(login.userName, "pql");
	strcpy(login.passWord, "pql1115");

	while (g_bRun)
	{
		for (int i = begin; i < end; i++)
		{
			client[i]->SendData(&login);
			//client[i]->OnRun();
		}
	}
	for (int i = begin; i < end; i++)
	{
		client[i]->Close();
	}
}

int main()
{

	//启动线程
	std::thread t1(CmdTread);
	t1.detach();

	for (int i = 0; i < tCount; i++)
	{
		std::thread t1(SendThread, i+1);
		t1.detach();
	}

	while (g_bRun)
	{
		Sleep(100);
	}

	getchar();

	return 0;
}