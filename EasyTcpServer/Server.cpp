/**
* @file		Server.cpp
* @brief		用Socket API 实现一个简单的TCP服务端
* @copyright	Copyright (c) 2020 ZhangSS
* @par 修改记录:
* <table>
* <tr><th>修改日期    <th>修改人    <th>描述
* <tr><td>2020-12-25  <td>ZhangSS  <td>创建第一版
* <tr><td>2021-03-05  <td>ZhangSS  <td>支持Linux系统
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