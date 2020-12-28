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

	printf("New Client Socket Join��socket = %d, IP = %s \n", _cSocket, inet_ntoa(clientAddr.sin_addr));

	char _recvBuf[128] = {};

	while (true)
	{
		/**
		* 5: recv ���տͻ�������
		*/
		int nLen = recv(_cSocket, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			printf("Client Close, Exit!!!\n");
			break;
		}
		printf("recv client data : %s \n", _recvBuf);
		/**
		* 6: ��������
		*/
		if (0 == strcmp(_recvBuf, "getName"))
		{
			/**
			* 7: send ��ͻ��˷���һ������
			*/
			char msgBuf[] = "Xiao qiang.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge"))
		{
			/**
			* 7: send ��ͻ��˷���һ������
			*/
			char msgBuf[] = "80.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else
		{
			/**
			* 7: send ��ͻ��˷���һ������
			*/
			char msgBuf[] = "???.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
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