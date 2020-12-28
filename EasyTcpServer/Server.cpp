/**
* @file		Server.cpp
* @brief		用Socket API 实现一个简单的TCP服务端
* @copyright	Copyright (c) 2020 ZhangSS
* @par 修改记录:
* <table>
* <tr><th>修改日期    <th>修改人    <th>描述
* <tr><td>2020-12-25  <td>ZhangSS  <td>创建第一版
* <tr><td>2020-12-25  <td>  <td>
* </table>
*/
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	/**
	* 1: 建立一个socket套接字
	*/
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/**
	* 2: bind 绑定用于接收客户端连接的网络端口
	*/
	sockaddr_in _sockaddr = {};
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(4567);
	_sockaddr.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1")
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sockaddr, sizeof(sockaddr_in)))
	{
		printf("ERROR: Bind Port Failed...\n");
	}
	else
	{
		printf("Bind Port Success...\n");
	}

	/**
	* 3: listen 监听网络端口
	*/
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR: Listen Port Failed...\n");
	}
	else
	{
		printf("Listen Port Success...\n");
	}

	/**
	* 4: accept 等待接受客户端连接
	*/
	sockaddr_in clientAddr = {};
	int clientAddrLen = sizeof(clientAddr);
	SOCKET _cSocket = INVALID_SOCKET;

	_cSocket = accept(_sock, (sockaddr*)&clientAddr, &clientAddrLen);
	if (SOCKET_ERROR == _cSocket)
	{
		printf("ERROR: Accept a Invalid Socket...\n");
	}

	printf("New Client Socket Join：socket = %d, IP = %s \n", _cSocket, inet_ntoa(clientAddr.sin_addr));

	char _recvBuf[128] = {};

	while (true)
	{
		/**
		* 5: recv 接收客户端数据
		*/
		int nLen = recv(_cSocket, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			printf("Client Close, Exit!!!\n");
			break;
		}
		printf("recv client data : %s \n", _recvBuf);
		/**
		* 6: 处理请求
		*/
		if (0 == strcmp(_recvBuf, "getName"))
		{
			/**
			* 7: send 向客户端发送一条数据
			*/
			char msgBuf[] = "Xiao qiang.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge"))
		{
			/**
			* 7: send 向客户端发送一条数据
			*/
			char msgBuf[] = "80.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else
		{
			/**
			* 7: send 向客户端发送一条数据
			*/
			char msgBuf[] = "???.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
	
	/**
	* 8: 关闭套接字closesocket
	*/
	closesocket(_sock);

	WSACleanup();

	printf("Server Exit!!!\n");

	getchar();

	return 0;
}