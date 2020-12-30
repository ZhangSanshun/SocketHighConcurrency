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

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

//消息头
struct DataHeader
{
	short cmd;
	short dataLenth;
};

//登录数据包
struct LoginData
{
	char userName[32];
	char passWord[32];
};

//登录结果数据包
struct LoginResultData
{
	int result;
};

//登出数据包
struct LogoutData
{
	char userName[32];
};

//登出结果数据包
struct LogoutResultData
{
	int result;
};

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
		else if (0 == strcmp(cmdBuf, "login"))
		{
			/**
			* 5: 向服务器发送请求命令
			*/
			LoginData loginData = { "zss", "123456" };
			DataHeader dataHeader = { CMD_LOGIN, sizeof(LoginData) };
			send(_sock, (const char*)&dataHeader, sizeof(DataHeader), 0);
			send(_sock, (const char*)&loginData, sizeof(LoginData), 0);

			/**
			* 6: 接收服务器信息 recv
			*/

			DataHeader dataHeaderRet = {};
			LoginResultData loginResultData = {};

			recv(_sock, (char*)&dataHeaderRet, sizeof(DataHeader), 0);
			recv(_sock, (char*)&loginResultData, sizeof(LoginResultData), 0);
			
			printf("recv loginResult data : %d\n", loginResultData.result);
			
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			/**
			* 5: 向服务器发送请求命令
			*/
			LogoutData logoutData = { "zss"};
			DataHeader dataHeader = { CMD_LOGOUT, sizeof(LogoutData) };

			send(_sock, (const char*)&dataHeader, sizeof(DataHeader), 0);
			send(_sock, (const char*)&logoutData, sizeof(LogoutData), 0);

			/**
			* 6: 接收服务器信息 recv
			*/
			DataHeader dataHeaderRet = {};
			LogoutResultData logoutResultData = {};

			recv(_sock, (char*)&dataHeaderRet, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutResultData, sizeof(LogoutResultData), 0);

			printf("recv logoutResult data : %d\n", logoutResultData.result);
		}
		else
		{
			printf("Input cmd Not Support!!! \n");
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