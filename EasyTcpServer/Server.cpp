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
#include <vector>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

//消息头
struct DataHeader
{
	short cmd;
	short dataLenth;
};

//登录数据包
struct LoginData : public DataHeader
{
	LoginData()
	{
		cmd = CMD_LOGIN;
		dataLenth = sizeof(LoginData);
	}

	char userName[32];
	char passWord[32];
};

//登录结果数据包
struct LoginResultData : public DataHeader
{
	LoginResultData()
	{
		cmd = CMD_LOGIN_RESULT;
		dataLenth = sizeof(LoginResultData);

		result = 0;
	}

	int result;
};

//登出数据包
struct LogoutData : public DataHeader
{
	LogoutData()
	{
		cmd = CMD_LOGOUT;
		dataLenth = sizeof(LogoutData);
	}

	char userName[32];
};

//登出结果数据包
struct LogoutResultData : public DataHeader
{
	LogoutResultData()
	{
		cmd = CMD_LOGOUT_RESULT;
		dataLenth = sizeof(LogoutResultData);

		result = 0;
	}

	int result;
};

//新用户加入数据包
struct NewUserJoinData : public DataHeader
{
	NewUserJoinData()
	{
		cmd = CMD_NEW_USER_JOIN;
		dataLenth = sizeof(NewUserJoinData);
		scok = 0;
	}

	int scok;
};

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSocket)
{
	/**
	* 5: recv 接收客户端数据
	*/
	//字符缓存区
	char szRecv[1024] = {};
	int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);

	DataHeader* header = (DataHeader*)szRecv;

	if (nLen <= 0)
	{
		printf("Client Close, Exit!!!\n");
		return -1;
	}

	/**
	* 6: 处理请求
	*/
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		//已经读取过消息头，需要进行偏移处理
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		LoginData* loginData;
		loginData = (LoginData*)szRecv;

		printf("recv client data : CMD_LOGIN , data length = %d, userName = %s, passWord = %s \n", loginData->dataLenth, loginData->userName, loginData->passWord);

		//忽略判断过程

		/**
		* 7: send 向客户端发送一条数据
		*/
		LoginResultData loginResultData;
		send(_cSocket, (char*)&loginResultData, sizeof(LoginResultData), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		LogoutData *logoutData;
		logoutData = (LogoutData*)szRecv;

		printf("recv client data : CMD_LOGOUT , data length = %d, userName = %s \n", logoutData->dataLenth, logoutData->userName);

		//忽略判断过程

		/**
		* 7: send 向客户端发送一条数据
		*/
		LogoutResultData logoutResultData;
		send(_cSocket, (char*)&logoutResultData, sizeof(LogoutResultData), 0);
	}
	break;
	default:
	{
		DataHeader header = { CMD_ERROR, 0 };
		send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
	}
	break;
	}

	return 0;
}

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

	while (true)
	{
		//select
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int i = (int)g_clients.size() - 1; i >= 0; i--)
		{
			FD_SET(g_clients[i], &fdRead);
		}

		//nfds 是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
		//即所有文件描述符最大值+1，在Windows中可以为0
		timeval t = {1, 0}; //非阻塞
		int  ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t); // timeval 为NULL时为阻塞
		if (ret < 0)
		{
			printf("select Exist!!! \n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

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

			//群发给现有客户端
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				NewUserJoinData newUserJoinData;
				newUserJoinData.scok = _cSocket;
				send(g_clients[i], (const char*)&newUserJoinData, sizeof(NewUserJoinData), 0);
			}

			g_clients.push_back(_cSocket);

			printf("New Client Socket Join: socket = %d, IP = %s \n", _cSocket, inet_ntoa(clientAddr.sin_addr));
		}

		for (size_t i = 0; i < fdRead.fd_count; i++)
		{
			if (-1 == processor(fdRead.fd_array[i]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[i]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}

		//printf("waiting.....\n");

	}

	for (size_t i = g_clients.size() - 1; i >= 0; i--)
	{
		closesocket(g_clients[i]);
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