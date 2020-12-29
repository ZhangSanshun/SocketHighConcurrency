/**
* @file		Client.cpp
* @brief		
* @copyright	Copyright (c) 2020 ZhangSS
* @par 修改记录:
* <table>
* <tr><th>修改日期    <th>修改人    <th>描述
* <tr><td>2020-12-28  <td>ZhangSS  <td>创建第一版
* <tr><td>2020-12-28  <td>  <td>
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
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("ERROR: Build Socket Failed...\n");
	}
	else
	{
		printf("Build Socket Success...\n");
	}
	/**
	* 2: 连接服务器 connect
	*/
	sockaddr_in _sockaddr = {};
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(4567);
	_sockaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sockaddr, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		printf("ERROR: Connect Server Failed...\n");
	}
	else
	{
		printf("Connect Server Success...\n");
	}

	
	while (true)
	{
		/**
		* 3: 输入请求命令
		*/
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);

		/**
		* 4: 处理请求
		*/
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("Input Eixt!!!\n");
			break;
		}
		else
		{
			/**
			* 5: 向服务器发送请求命令
			*/
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}
		
		/**
		* 6: 接收服务器信息 recv
		*/
		char recvBuf[256] = {};
		int nlen = recv(_sock, recvBuf, 256, 0);
		if (nlen > 0)
		{
			printf("recv data : %s \n", recvBuf);
		}
	}

	/**
	* 7: 关闭套接字closesocket
	*/
	closesocket(_sock);

	WSACleanup();

	printf("Client Exit!!!\n");

	getchar();  

	return 0;
}