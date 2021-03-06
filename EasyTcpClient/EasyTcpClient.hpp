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

	//��ʼ��socket
	void InitSocket()
	{
		//����Win Sock 2.x����
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		/**
		* 1: ����һ��socket�׽���
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
			printf("Build Socket Success...\n");
		}
	}

	//���ӷ�����
	int Connect(char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}

		/**
		* 2: ���ӷ����� connect
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
			printf("Connect Server Success...\n");
		}

		return ret;
	}

	//�ر�socket
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


	//��ѯ������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);

			timeval t;
			t.tv_sec = 1;
			t.tv_usec = 0;

			int ret = select(_sock + 1, &fdReads, nullptr, nullptr, &t);

			if (ret < 0)
			{
				printf("[socket=%d]select close1!!!\n", _sock);
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);

				if (-1 == RecvData(_sock))
				{
					printf("[socket=%d]select close2!!!\n", _sock);
					return false;
				}
			}

			return true;
		}

		return false;
	}

	//�Ƿ�����
	bool isRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//��������,����ճ������ְ�
	int RecvData(SOCKET _cSocket)
	{
		/**
		* 5: recv ���տͻ�������
		*/
		//�ַ�������
		char szRecv[1024] = {};
		int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);

		DataHeader* header = (DataHeader*)szRecv;

		if (nLen <= 0)
		{
			printf("communication break, finish!!!\n");
			return -1;
		}

		//�Ѿ���ȡ����Ϣͷ����Ҫ����ƫ�ƴ���
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		OnNetMsg(header);

		return 0;
	}

	//��Ӧ������Ϣ
	void OnNetMsg(DataHeader* header)
	{
		/**
		* 6: ��������
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
		}
	}

	//��������
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

