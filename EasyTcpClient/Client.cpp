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

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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

	
	while (true)
	{
		/**
		* 3: ������������
		*/
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);

		/**
		* 4: ��������
		*/
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("Input Eixt!!!\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			/**
			* 5: �������������������
			*/
			LoginData loginData;
			strcpy(loginData.userName, "zss");
			strcpy(loginData.passWord, "123456");

			send(_sock, (const char*)&loginData, sizeof(LoginData), 0);

			/**
			* 6: ���շ�������Ϣ recv
			*/
			LoginResultData loginResultData = {};

			recv(_sock, (char*)&loginResultData, sizeof(LoginResultData), 0);
			
			printf("recv loginResult data : %d\n", loginResultData.result);
			
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			/**
			* 5: �������������������
			*/
			LogoutData logoutData;
			strcpy(logoutData.userName, "zss");
			
			send(_sock, (const char*)&logoutData, sizeof(LogoutData), 0);

			/**
			* 6: ���շ�������Ϣ recv
			*/
			LogoutResultData logoutResultData = {};

			recv(_sock, (char*)&logoutResultData, sizeof(LogoutResultData), 0);

			printf("recv logoutResult data : %d\n", logoutResultData.result);
		}
		else
		{
			printf("Input cmd Not Support!!! \n");
		}
		
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