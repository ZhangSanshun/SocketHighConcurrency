#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define FD_SETSIZE      1024

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
#include <thread>
#include <mutex>
#include <atomic>

#include "MessageHeader.hpp"
#include "CELLTimestamp.hpp"

//��������С��Ԫ��С
#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif // !RECV_BUFF_SIZE

#define CELLSERVER_THREAD_COUNT 4

class ClientSocket
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}

	SOCKET sockfd()
	{
		return _sockfd;
	}

	char* msgBuf()
	{
		return _szMsgBuf;
	}

	int getLastPos()
	{
		return _lastPos;
	}

	void setLastPos(int pos)
	{
		_lastPos = pos;
	}

private:
	SOCKET _sockfd; //socket fd_set 
	//��Ϣ������
	char _szMsgBuf[RECV_BUFF_SIZE * 10];
	//��Ϣ������β��λ��
	int _lastPos;
};

class INetEvent
{
public:
	virtual void OnLeave(ClientSocket* pClient) = 0;
private:
};

class CellServer
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET) 
	{
		_sock = sock;
		_pThread = nullptr;
		_recvCount = 0;
		_pNetEvent = nullptr;
	}

	~CellServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

	void SetEventObj(INetEvent* pNetEvent)
	{
		_pNetEvent = pNetEvent;
	}

	//����������Ϣ
	bool OnRun()
	{
		while (isRun())
		{
			//������ͻ������еĿͻ����뵽��ʽ�ͻ�������
			if (_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(_mutex);

				for (auto pClient : _clientsBuff)
				{
					_clients.push_back(pClient);
				}
				_clientsBuff.clear();
			}

			if (_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);

				continue;
			}

			//select
			fd_set fdRead;
		
			FD_ZERO(&fdRead);
			
			SOCKET maxSock = _clients[0]->sockfd();

			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				FD_SET(_clients[i]->sockfd(), &fdRead);
				if (maxSock < _clients[i]->sockfd())
				{
					maxSock = _clients[i]->sockfd();
				}
			}

			//nfds ��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
			//�������ļ����������ֵ+1����Windows�п���Ϊ0
			timeval t = { 0, 0 }; //������
			int  ret = select(maxSock + 1, &fdRead, 0,  0, &t); // timeval ΪNULLʱΪ����
			if (ret < 0)
			{
				printf("select Exist!!! \n");
				Close();
				return false;
			}

			//Support Linux
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				if (FD_ISSET(_clients[i]->sockfd(), &fdRead))
				{
					if (-1 == RecvData(_clients[i]))
					{
						auto iter = _clients.begin() + i;
						if (iter != _clients.end())
						{
							if (_pNetEvent)
							{
								_pNetEvent->OnLeave(_clients[i]);
							}
							delete _clients[i];
							_clients.erase(iter);
						}
					}
				}
			}
		}

	}

	//�Ƿ�����
	bool isRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//�ر�socket
	void Close()
	{

		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			/**
			* 8: �ر��׽���closesocket
			*/
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				closesocket(_clients[i]->sockfd());
				delete _clients[i];
			}

			closesocket(_sock);

			WSACleanup();
#else
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				close(_clients[i]->sockfd());
				delete _clients[i];
			}

			close(_sock);
#endif
			_clients.clear();
		}
	}

	//������
	char _szRecv[RECV_BUFF_SIZE] = {};

	//�������ݣ�����ճ������ְ�
	int RecvData(ClientSocket* pClient)
	{
		// 5: recv ���տͻ�������
		int nLen = recv(pClient->sockfd(), _szRecv, RECV_BUFF_SIZE, 0);

		DataHeader* header = (DataHeader*)_szRecv;

		if (nLen <= 0)
		{
			printf("Client Close, Exit!!!\n");
			return -1;
		}

		//����ȡ�������ݿ�������Ϣ������
		memcpy(pClient->msgBuf() + pClient->getLastPos(), _szRecv, nLen);
		//��Ϣ������������β��λ�ú���
		pClient->setLastPos(pClient->getLastPos() + nLen);
		//�ж��ѽ��յĻ�������Ϣ���ݳ��ȴ���DataHeader����
		while (pClient->getLastPos() > sizeof(DataHeader))
		{
			//��ʱ�Ϳ���֪����ǰ��Ϣ��ĳ���
			DataHeader* header = (DataHeader*)pClient->msgBuf();
			//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
			if (pClient->getLastPos() >= header->dataLenth)
			{
				//��Ϣ������ʣ��δ�������ݵĳ���
				int nSize = pClient->getLastPos() - header->dataLenth;
				//����������Ϣ
				OnNetMsg(pClient->sockfd(), header);
				//����Ϣ������ʣ��δ��������ǰ��
				memcpy(pClient->msgBuf(), pClient->msgBuf() + header->dataLenth, nSize);
				//��Ϣ������������β��ǰ��
				pClient->setLastPos(nSize);
			}
			else
			{
				//��Ϣ������ʣ�����ݲ���һ��������Ϣ
				break;
			}
		}

		return 0;
	}

	//��Ӧ������Ϣ
	void OnNetMsg(SOCKET _cSocket, DataHeader* header)
	{
		_recvCount++;

		/*auto t1 = _tTime.GetElapsedSecond();
		if (t1 >= 1.0)
		{
			printf("time<%lf>, clients<%d>, _recvCount<%d>\n", t1, _clients.size(), _recvCount);
			_tTime.Update();
			_recvCount = 0;
		}*/

		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			LoginData* loginData;
			loginData = (LoginData*)header;

			//printf("recv client data : CMD_LOGIN , data length = %d, userName = %s, passWord = %s \n", loginData->dataLenth, loginData->userName, loginData->passWord);

			//LoginResultData loginResultData;

			//SendData(_cSocket, &loginResultData);
		}
		break;
		case CMD_LOGOUT:
		{
			LogoutData *logoutData;
			logoutData = (LogoutData*)header;

			//printf("recv client data : CMD_LOGOUT , data length = %d, userName = %s \n", logoutData->dataLenth, logoutData->userName);

			LogoutResultData logoutResultData;

			//SendData(_cSocket, &logoutResultData);
		}
		break;
		default:
		{
			printf("recv unknow data , data length = %d \n", header->dataLenth);

			DataHeader header;

			//SendData(_cSocket, &header);
		}
		break;
		}
	}

	void AddClient(ClientSocket* pClient)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		_clientsBuff.push_back(pClient);
	}

	void Start()
	{
		_pThread = new std::thread(std::mem_fun(&CellServer::OnRun), this);
		_pThread->detach();
	}

	size_t GetClientCount()
	{
		return _clients.size() + _clientsBuff.size();
	}

public:

	std::atomic_int _recvCount;

private:

	SOCKET _sock;
	//��ʽ�ͻ�����
	std::vector<ClientSocket*> _clients;
	//����ͻ�����
	std::vector<ClientSocket*> _clientsBuff;

	std::mutex _mutex;

	std::thread* _pThread;

	INetEvent* _pNetEvent;

};

class EasyTcpServer : public INetEvent
{

private:

	SOCKET _sock;

	std::vector<ClientSocket*> _clients;

	std::vector<CellServer*> _cellServers;

	CELLTimestamp _tTime;

public:
	EasyTcpServer() 
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpServer()
	{
		Close();
	}

	void OnLeave(ClientSocket* pClient)
	{
		for (int i = _clients.size() - 1; i >= 0; i--)
		{
			if (_clients[i] == pClient)
			{
				auto iter = _clients.begin() + i;
				if (iter != _clients.end())
				{
					_clients.erase(iter);
				}
			}
		}
	}

	//��ʼ��socket
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

	//��ip�Ͷ˿ں�
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

	//�����˿ں�
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

	//���տͻ�������
	SOCKET Accept()
	{
		sockaddr_in clientAddr = {};
		int clientAddrLen = sizeof(clientAddr);
		SOCKET cSock = INVALID_SOCKET;
#ifdef _WIN32
		cSock = accept(_sock, (sockaddr*)&clientAddr, &clientAddrLen);
#else
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&clientAddrLen);
#endif
		if (SOCKET_ERROR == cSock)
		{
			printf("ERROR: [socket=%d]Accept a Invalid Socket...\n", _sock);
		}
		else
		{
			NewUserJoinData newUserJoinData;

			SendDataToAll(&newUserJoinData);

			AddClientToCellServer(new ClientSocket(cSock));

			//printf("[socket=%d]New Client Socket Join: socket = %d, IP = %s \n", _sock, cSock, inet_ntoa(clientAddr.sin_addr));
		}

		return cSock;
	}

	void AddClientToCellServer(ClientSocket* pClient)
	{
		_clients.push_back(pClient);

		//���ҿͻ��������ٵ�CellServer��Ϣ�������
		auto pMinServer = _cellServers[0];
		for (auto pCellServer : _cellServers)
		{
			if (pMinServer->GetClientCount() > pCellServer->GetClientCount())
			{
				pMinServer = pCellServer;
			}
		}
		pMinServer->AddClient(pClient);
	}

	void Start()
	{

		for (int i = 0; i < CELLSERVER_THREAD_COUNT; i ++)
		{
			auto ser = new CellServer(_sock);
			_cellServers.push_back(ser);

			ser->SetEventObj(this);
			ser->Start();
		}
	}

	void TimeForMsg()
	{
		auto t1 = _tTime.GetElapsedSecond();
		if (t1 >= 1.0)
		{
			int recvCount = 0;
			for (auto ser : _cellServers)
			{
				recvCount += ser->_recvCount;
				ser->_recvCount = 0;
			}

			printf("time<%lf>, clients<%d>, _recvCount<%d>\n", t1, _clients.size(), (int)recvCount);
			_tTime.Update();
		}
	}

	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			TimeForMsg();

			//select
			fd_set fdRead;
			//fd_set fdWrite;
			//fd_set fdExp;

			FD_ZERO(&fdRead);
		//	FD_ZERO(&fdWrite);
			//FD_ZERO(&fdExp);

			FD_SET(_sock, &fdRead);
			//FD_SET(_sock, &fdWrite);
			//FD_SET(_sock, &fdExp);

			//nfds ��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
			//�������ļ����������ֵ+1����Windows�п���Ϊ0
			timeval t = { 0, 0 }; //������
			int  ret = select(_sock + 1, &fdRead, 0, 0, &t); // timeval ΪNULLʱΪ����
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

		}
		return true;
	}

	//��������
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
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				SendData(_clients[i]->sockfd(), header);
			}
		}
	}

	//�Ƿ�����
	bool isRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//�ر�socket
	void Close()
	{

		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			/**
			* 8: �ر��׽���closesocket
			*/
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				closesocket(_clients[i]->sockfd());
				delete _clients[i];
			}

			closesocket(_sock);

			WSACleanup();
#else
			for (int i = (int)_clients.size() - 1; i >= 0; i--)
			{
				close(_clients[i]->sockfd());
				delete _clients[i];
			}

			close(_sock);
#endif
			_clients.clear();
		}
	}

};



#endif  // !_EasyTcpServer_hpp_