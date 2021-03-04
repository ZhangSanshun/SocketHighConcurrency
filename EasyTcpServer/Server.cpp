/**
* @file		Server.cpp
* @brief		��Socket API ʵ��һ���򵥵�TCP�����
* @copyright	Copyright (c) 2020 ZhangSS
* @par �޸ļ�¼:
* <table>
* <tr><th>�޸�����    <th>�޸���    <th>����
* <tr><td>2020-12-25  <td>ZhangSS  <td>������һ��
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

//��Ϣͷ
struct DataHeader
{
	short cmd;
	short dataLenth;
};

//��¼���ݰ�
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

//��¼������ݰ�
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

//�ǳ����ݰ�
struct LogoutData : public DataHeader
{
	LogoutData()
	{
		cmd = CMD_LOGOUT;
		dataLenth = sizeof(LogoutData);
	}

	char userName[32];
};

//�ǳ�������ݰ�
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

//���û��������ݰ�
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
	* 5: recv ���տͻ�������
	*/
	//�ַ�������
	char szRecv[1024] = {};
	int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);

	DataHeader* header = (DataHeader*)szRecv;

	if (nLen <= 0)
	{
		printf("Client Close, Exit!!!\n");
		return -1;
	}

	/**
	* 6: ��������
	*/
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		//�Ѿ���ȡ����Ϣͷ����Ҫ����ƫ�ƴ���
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		LoginData* loginData;
		loginData = (LoginData*)szRecv;

		printf("recv client data : CMD_LOGIN , data length = %d, userName = %s, passWord = %s \n", loginData->dataLenth, loginData->userName, loginData->passWord);

		//�����жϹ���

		/**
		* 7: send ��ͻ��˷���һ������
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

		//�����жϹ���

		/**
		* 7: send ��ͻ��˷���һ������
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
	* 1: ����һ��socket�׽���
	*/
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/**
	* 2: bind �����ڽ��տͻ������ӵ�����˿�
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
	* 3: listen ��������˿�
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

		//nfds ��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
		//�������ļ����������ֵ+1����Windows�п���Ϊ0
		timeval t = {1, 0}; //������
		int  ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t); // timeval ΪNULLʱΪ����
		if (ret < 0)
		{
			printf("select Exist!!! \n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

			/**
			* 4: accept �ȴ����ܿͻ�������
			*/
			sockaddr_in clientAddr = {};
			int clientAddrLen = sizeof(clientAddr);
			SOCKET _cSocket = INVALID_SOCKET;

			_cSocket = accept(_sock, (sockaddr*)&clientAddr, &clientAddrLen);
			if (SOCKET_ERROR == _cSocket)
			{
				printf("ERROR: Accept a Invalid Socket...\n");
			}

			//Ⱥ�������пͻ���
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
	* 8: �ر��׽���closesocket
	*/
	closesocket(_sock);

	WSACleanup();

	printf("Server Exit!!!\n");

	getchar();

	return 0;
}