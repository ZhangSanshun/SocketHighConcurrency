/**
* @file		Client.cpp
* @brief		
* @copyright	Copyright (c) 2020 ZhangSS
* @par �޸ļ�¼:
* <table>
* <tr><th>�޸�����    <th>�޸���    <th>����
* <tr><td>2020-12-28  <td>ZhangSS  <td>������һ��
* <tr><td>2020-12-28  <td>  <td>
* </table>
*/

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>

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
		printf("communication break, finish!!!\n");
		return -1;
	}

	/**
	* 6: ��������
	*/
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		//�Ѿ���ȡ����Ϣͷ����Ҫ����ƫ�ƴ���
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		LoginResultData* loginResultData;
		loginResultData = (LoginResultData*)szRecv;

		printf("recv service data : CMD_LOGIN_RESULT , data length = %d\n", loginResultData->dataLenth);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		LogoutResultData *logoutResultData;
		logoutResultData = (LogoutResultData*)szRecv;

		printf("recv service data : CMD_LOGOUT_RESULT , data length = %d \n", logoutResultData->dataLenth);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);

		NewUserJoinData *newUserJoinData;
		newUserJoinData = (NewUserJoinData*)szRecv;

		printf("recv service data : CMD_NEW_USER_JOIN ,socket = %d, data length = %d \n", newUserJoinData->scok, newUserJoinData->dataLenth);
	}
	break;
	}

	return 0;
}

bool g_bRun = true;

void CmdTread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);

		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("Exit thread!!!\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			LoginData login;
			strcpy(login.userName, "pql");
			strcpy(login.passWord, "pql1115");

			send(_sock, (const char*)&login, sizeof(LoginData), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LogoutData logout;
			strcpy(logout.userName, "pql");

			send(_sock, (const char*)&logout, sizeof(LogoutData), 0);
		}
		else
		{
			printf("input data error!!!\n");
		}
	}

}

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	/**
	* 1: ����һ��socket�׽���
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
	* 2: ���ӷ����� connect
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

	//�����߳�
	std::thread t1(CmdTread, _sock);
	t1.detach();
	
	while (g_bRun)
	{

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);

		timeval t;
		t.tv_sec = 1;
		t.tv_usec = 0;

		int ret = select(_sock, &fdReads, nullptr, nullptr, &t);

		if (ret < 0)
		{
			printf("select close1!!!\n");
			break;
		}

		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);

			if (-1 == processor(_sock))
			{
				printf("select close2!!!\n");
				break;
			}
		}



		/*printf("waiting.....\n");

		LoginData login;
		strcpy(login.userName, "pql");
		strcpy(login.passWord, "pql1115");

		send(_sock, (const char*)&login, sizeof(LoginData), 0);

		Sleep(2000);*/
		
	}

	/**
	* 7: �ر��׽���closesocket
	*/
	closesocket(_sock);

	WSACleanup();

	printf("Client Exit!!!\n");

	getchar();

	return 0;
}