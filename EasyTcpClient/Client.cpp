/**
* @file		Client.cpp
* @brief
* @copyright	Copyright (c) 2020 ZhangSS
* @par �޸ļ�¼:
* <table>
* <tr><th>�޸�����    <th>�޸���    <th>����
* <tr><td>2020-12-28  <td>ZhangSS  <td>������һ��
* <tr><td>2021-03-05  <td>ZhangSS  <td>֧��Linuxϵͳ
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

//�ͻ�������
const int cCount = 1000;
//�߳�����
const int tCount = 4;

//�ͻ����߳�
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

	//�����߳�
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