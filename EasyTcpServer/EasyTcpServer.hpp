#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif

#include <iostream>
#include <vector>

#include "MessageHeader.hpp"


class EasyTcpServer
{
public:
	EasyTcpServer() 
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}

	//初始化socket
	SOCKET InitSocket()
	{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		if (INVALID_SOCKET != _sock)
		{
			printf("[socket=%d]Close old connect...\n", _sock);
			Close();
		}

		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == _sock)
		{
			printf("ERROR: Build Socket[socket=%d] Failed...\n", _sock);
		}
		else
		{
			printf("Build Socket[socket=%d] Success...\n", _sock);
		}

		return _sock;
	}

	//绑定ip和端口号
	int Bind(char * ip, unsigned short port)
	{
		sockaddr_in _sockaddr = {};
		_sockaddr.sin_family = AF_INET;
		_sockaddr.sin_port = htons(port);

#ifdef _WIN32	
		if (ip)
		{
			_sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sockaddr.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1")
		}
#else
		if (ip)
		{
			_sockaddr.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			_sockaddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1")
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sockaddr, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("ERROR: Bind Port [%d] Failed...\n", port);
		}
		else
		{
			printf("Bind Port [%d] Success...\n", port);
		}

		return ret;
	}

	//监听端口号
	int Listen(int n)
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			printf("ERROR: [socket=%d]Listen Port Failed...\n", _sock);
		}
		else
		{
			printf("[socket=%d]Listen Port Success...\n", _sock);
		}

		return ret;
	}

	//接收客户端连接
	SOCKET Accept()
	{
		sockaddr_in clientAddr = {};
		int clientAddrLen = sizeof(clientAddr);
		SOCKET _cSocket = INVALID_SOCKET;
#ifdef _WIN32
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, &clientAddrLen);
#else
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&clientAddrLen);
#endif
		if (SOCKET_ERROR == _cSocket)
		{
			printf("ERROR: [socket=%d]Accept a Invalid Socket...\n", _sock);
		}
		else
		{
			NewUserJoinData newUserJoinData;

			SendDataToAll(&newUserJoinData);

			g_clients.push_back(_cSocket);

			printf("[socket=%d]New Client Socket Join: socket = %d, IP = %s \n", _sock, _cSocket, inet_ntoa(clientAddr.sin_addr));
		}

		return _cSocket;
	}

	//关闭socket
	void Close()
	{

		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			/**
			* 8: 关闭套接字closesocket
			*/
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				closesocket(g_clients[i]);
			}

			closesocket(_sock);

			WSACleanup();
#else
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				close(g_clients[i]);
			}

			close(_sock);
#endif
		}
	}

	//处理网络消息
	bool OnRun()
	{
		if (isRun())
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

			SOCKET maxSock = _sock;

			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				FD_SET(g_clients[i], &fdRead);
				if (maxSock < g_clients[i])
				{
					maxSock = g_clients[i];
				}
			}

			//nfds 是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量
			//即所有文件描述符最大值+1，在Windows中可以为0
			timeval t = { 0, 0 }; //非阻塞
			int  ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t); // timeval 为NULL时为阻塞
			if (ret < 0)
			{
				printf("select Exist!!! \n");
				Close();
				return false;
			}

			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);

				Accept();
			}

			//Support Linux
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				if (FD_ISSET(g_clients[i], &fdRead))
				{
					if (-1 == RecvData(g_clients[i]))
					{
						auto iter = g_clients.begin() + i;
						if (iter != g_clients.end())
						{
							g_clients.erase(iter);
						}
					}
				}
			}
		}
	}

	//是否工作中
	bool isRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//接收数据，处理粘包、拆分包
	int RecvData(SOCKET _cSocket)
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

		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		OnNetMsg(_cSocket, header);

		return 0;
	}

	//响应网络消息
	void OnNetMsg(SOCKET _cSocket, DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{

			LoginData* loginData;
			loginData = (LoginData*)header;

			printf("recv client data : CMD_LOGIN , data length = %d, userName = %s, passWord = %s \n", loginData->dataLenth, loginData->userName, loginData->passWord);

			LoginResultData loginResultData;
			
			SendData(_cSocket, &loginResultData);
		}
		break;
		case CMD_LOGOUT:
		{

			LogoutData *logoutData;
			logoutData = (LogoutData*)header;

			printf("recv client data : CMD_LOGOUT , data length = %d, userName = %s \n", logoutData->dataLenth, logoutData->userName);

			LogoutResultData logoutResultData;

			SendData(_cSocket, &logoutResultData);
		}
		break;
		default:
		{
			DataHeader header = { CMD_ERROR, 0 };

			SendData(_cSocket, &header);
		}
		break;
		}
	}

	//发送数据
	int SendData(SOCKET _cSock, DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_cSock, (const char*)header, header->dataLenth, 0);
		}
		return SOCKET_ERROR;
	}

	void SendDataToAll(DataHeader* header)
	{
		if (isRun() && header)
		{
			for (int i = (int)g_clients.size() - 1; i >= 0; i--)
			{
				SendData(g_clients[i], header);
			}
		}
	}

private:

	SOCKET _sock;

	std::vector<SOCKET> g_clients;


};


#endif  // !_EasyTcpServer_hpp_