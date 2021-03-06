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

	//�����߳�
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