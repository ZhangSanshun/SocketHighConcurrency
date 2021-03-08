/**
* @file		Server.cpp
* @brief		��Socket API ʵ��һ���򵥵�TCP�����
* @copyright	Copyright (c) 2020 ZhangSS
* @par �޸ļ�¼:
* <table>
* <tr><th>�޸�����    <th>�޸���    <th>����
* <tr><td>2020-12-25  <td>ZhangSS  <td>������һ��
* <tr><td>2021-03-05  <td>ZhangSS  <td>֧��Linuxϵͳ
* </table>
*/


#include "EasyTcpServer.hpp"

int main()
{

	EasyTcpServer server;

	server.InitSocket();
	server.Bind(nullptr, 4567);
	server.Listen(5);

	while (server.isRun())
	{
		server.OnRun();
	}

	server.Close();

	printf("Server Exit!!!\n");

	getchar();

	return 0;
}