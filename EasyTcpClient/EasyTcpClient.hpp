#ifndef  _EasyTcpClient_hpp_
#define  _EasyTcpClient_hpp_

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
#include <thread>

#include "MessageHeader.hpp"

class EasyTcpClient
{
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpClient() 
	{
		Close();
	}

	//初始化socket
	void InitSocket()
	{
		//启动Win Sock 2.x环境
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		/**
		* 1: 建立一个socket套接字
		*/
		if (INVALID_SOCKET != _sock)
		{
			printf("[socket=%d]Close old connect...\n", _sock);
			Close();
		}

		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		
		if (INVALID_SOCKET == _sock)
		{
			printf("ERROR: Build Socket Failed...\n");
		}
		else
		{
			//printf("Build Socket Success...\n");
		}
	}

	//连接服务器
	int Connect(char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}

		/**
		* 2: 连接服务器 connect
		*/
		sockaddr_in _sockaddr = {};
		_sockaddr.sin_family = AF_INET;
		_sockaddr.sin_port = htons(port);
#ifdef _WIN32	
		_sockaddr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sockaddr.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sockaddr, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("ERROR: Connect Server Failed...\n");
		}
		else
		{
			//printf("Connect Server Success...\n");
		}

		return ret;
	}

	//关闭socket
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sock);
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;

			printf("Client Exit!!!\n");
		}
	}


	//查询网络消息
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);

			timeval t;
			t.tv_sec = 0;
			t.tv_usec = 0;

			int ret = select(_sock + 1, &fdReads, nullptr, nullptr, &t);

			if (ret < 0)
			{
				printf("[socket=%d]select close1!!!\n", _sock);
				Close();
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);

				if (-1 == RecvData(_sock))
				{
					printf("[socket=%d]select close2!!!\n", _sock);
					Close();
					return false;
				}
			}

			return true;
		}

		return false;
	}

	//是否工作中
	bool isRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//缓冲区最小单元大小
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 1024
#endif // !RECV_BUFF_SIZE


	//接收缓存区
	char _szRecv[RECV_BUFF_SIZE] = {};

	//消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};
	//消息缓冲区尾部位置
	int _lastPos = 0;

	//接收数据,处理粘包，拆分包
	int RecvData(SOCKET cSocket)
	{
		int nLen = recv(cSocket, _szRecv, RECV_BUFF_SIZE, 0);

		if (nLen <= 0)
		{
			printf("communication break, finish!!!\n");
			return -1;
		}

		//将收取到的数据拷贝到消息缓冲区
		memcpy(_szMsgBuf + _lastPos, _szRecv, nLen);
		//消息缓冲区的数据尾部位置后移
		_lastPos += nLen;
		//判断已接收的缓冲区消息数据长度大于DataHeader长度
		while (_lastPos > sizeof(DataHeader))
		{
			//这时就可以知道当前消息体的长度
			DataHeader* header = (DataHeader*)_szMsgBuf;
			//判断消息缓冲区的数据长度大于消息长度
			if (_lastPos >= header->dataLenth)
			{
				//消息缓冲区剩余未处理数据的长度
				int nSize = _lastPos - header->dataLenth;
				//处理网络消息
				OnNetMsg(header);
				//将消息缓冲区剩余未处理数据前移
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLenth, nSize);
				//消息缓冲区的数据尾部前移
				_lastPos = nSize;
			}
			else
			{
				//消息缓冲区剩余数据不够一条完整消息
				break;
			}
		}
		return 0;
	}

	//相应网络消息
	void OnNetMsg(DataHeader* header)
	{
		/**
		* 6: 处理请求
		*/
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResultData* loginResultData;
			loginResultData = (LoginResultData*)header;

			printf("recv service data : CMD_LOGIN_RESULT , data length = %d\n", loginResultData->dataLenth);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResultData *logoutResultData;
			logoutResultData = (LogoutResultData*)header;

			printf("recv service data : CMD_LOGOUT_RESULT , data length = %d \n", logoutResultData->dataLenth);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoinData *newUserJoinData;
			newUserJoinData = (NewUserJoinData*)header;

			printf("recv service data : CMD_NEW_USER_JOIN ,socket = %d, data length = %d \n", newUserJoinData->scok, newUserJoinData->dataLenth);
		}
		break;
		case CMD_ERROR:
		{
			printf("recv service data : CMD_ERROR , data length = %d \n", header->dataLenth);
		}
		break;
		default:
		{
			printf("recv unknow data , data length = %d \n", header->dataLenth);
		}
		}
		
	}

	//发送数据
	int SendData(DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLenth, 0);
		}
		return SOCKET_ERROR;
	}

private:

	SOCKET _sock;

};


#endif

