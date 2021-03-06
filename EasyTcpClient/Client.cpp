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


void CmdTread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[1024] = {};
		scanf("%s", cmdBuf);

		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->Close();
			printf("Exit thread!!!\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			LoginData login;
			strcpy(login.userName, "pql");
			strcpy(login.passWord, "pql1115");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LogoutData logout;
			strcpy(logout.userName, "pql");

			client->SendData(&logout);
		}
		else
		{
			printf("input data error!!!\n");
		}
	}

}

int main()
{
	EasyTcpClient client;

	//client.InitSocket();
	client.Connect("127.0.0.1", 4567);

	//启动线程
	std::thread t1(CmdTread, &client);
	t1.detach();

	while (client.isRun())
	{
		client.OnRun();
	}

	client.Close();


	getchar();

	return 0;
}