/**
 * @file		Socket.cpp
 * @brief		该文件是实现Socket.h文件的声明函数。
 * @copyright	Copyright (c) 2020 JackChen
 * @par 修改记录:
 * <table>
 * <tr><th>修改日期    <th>修改人    <th>描述
 * <tr><td>2020-09-02  <td>JackChen  <td>创建第一版
 * </table>
 */
#include "Socket.h"


#ifdef _WIN32
#define CONN_INPRROGRESS_FLAG	WSAEWOULDBLOCK
#define INVALID_SOCKET_FLAG		INVALID_SOCKET
#define SOCKET_ERROR_FLAG		SOCKET_ERROR

#define SOCKET_ERROR_CODE		WSAGetLastError()

#define Socket_Close_Func		closesocket
#define Socket_Ioctl_Func		ioctlsocket
#define MAX_CLIENT_NUM 64
#endif

#ifdef __linux__						
#define CONN_INPRROGRESS_FLAG	EINPROGRESS
#define INVALID_SOCKET_FLAG		(-1)
#define SOCKET_ERROR_FLAG		(-1)

#define SOCKET_ERROR_CODE		errno

#define Socket_Close_Func		close
#define Socket_Ioctl_Func		ioctl
#define MAX_CLIENT_NUM 1024
#endif

#define Socket_Socket_Func		socket
#define Socket_Accept_Func		accept
#define Socket_Bind_Func	    bind
#define Socket_Connect_Func		connect
#define Socket_Listen_Func		listen
#define Socket_Send_Func		send
#define Socket_Recv_Func		recv
#define Socket_Sendto_Func		sendto
#define Socket_Recvfrom_Func	recvfrom
#define Socket_Select_Func		select
#define Socket_Setsockopt_Func	setsockopt
#define Socket_Getsockopt_Func	getsockopt
#define Socket_Shutdown_Func	shutdown


namespace jc
{
    Socket::Socket(void)
    {
    }

    Socket& Socket::operator =(const Socket& obj)
    {
        if (this == &obj) {
            return *this;
        }

        return *this;
    }

    Socket::Socket(const Socket& obj)
    {
    }

    Socket::~Socket(void)
    {
    }

    bool Socket::SetSendBufferSize(int size)
    {
        int ret = -1;
        length_var length = (length_var)size;
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDBUF, (char*)&length, sizeof(length));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetRecvBufferSize(int size)
    {
        int ret = -1;
        length_var length = (length_var)size;
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVBUF, (char*)&length, sizeof(length));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::GetSendBufferSize(int &size)
    {
        int ret = -1;
        length_var length = (length_var)size;
        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDBUF, (char*)&size, &length);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::GetRecvBufferSize(int &size)
    {
        int ret = -1;
        length_var length = (length_var)size;
        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVBUF, (char*)&size, &length);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetRecvTimeOut(int ms)
    {
        int ret = -1;
#ifdef _WIN32
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&ms, sizeof(ms));
#endif
#ifdef __linux__
        struct timeval t = { ms / 1000, (ms % 1000) * 1000 };
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, sizeof(t));
#endif
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetSendTimeOut(int ms)
    {
        int ret = -1;
#ifdef _WIN32
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&ms, sizeof(ms));
#endif
#ifdef __linux__
        struct timeval t = { ms / 1000, (ms % 1000) * 1000 };
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, sizeof(t));
#endif
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::GetRecvTimeOut(int &ms)
    {
        int ret = -1;
#ifdef _WIN32
        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&ms, &ms);
#endif
#ifdef __linux__
        struct timeval t;
        length_var length = sizeof(timeval);

        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&t, &length);
        if (SOCKET_ERROR_FLAG != ret) {
            ms = t.tv_sec * 1000 + t.tv_usec / 1000;
        }
#endif
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::GetSendTimeOut(int &ms)
    {
        int ret = -1;
#ifdef _WIN32
        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&ms, &ms);
#endif
#ifdef __linux__
        struct timeval t;
        length_var length = sizeof(timeval);

        ret = Socket_Getsockopt_Func(this->m_sSock, SOL_SOCKET, SO_SNDTIMEO, (char*)&t, &length);
        if (SOCKET_ERROR_FLAG != ret) {
            ms = t.tv_sec * 1000 + t.tv_usec / 1000;
        }
#endif
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetBlock(bool bIsBlock)
    {
        unsigned long arg = bIsBlock ? 0 : 1;
        int ret = -1;
        ret = Socket_Ioctl_Func(this->m_sSock, FIONBIO, &arg);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetReuseAddr(bool bIsReuse)
    {
        int arg = bIsReuse ? 1 : 0;
        int ret = -1;
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_REUSEADDR, (char*)&arg, sizeof(arg));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetSendDelay(bool isDelay)
    {
        int arg = isDelay ? 0 : 1;
        int ret = -1;
#ifdef _WIN32
        ret = Socket_Setsockopt_Func(this->m_sSock, IPPROTO_TCP, TCP_NODELAY, (char*)&arg, sizeof(arg));
#endif
#ifdef linux
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_TCP, TCP_NODELAY, (char*)&arg, sizeof(arg));
#endif
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::SetLinger(int ms)
    {
        int ret = -1;
        struct linger lt = { u_short(ms / 1000), u_short((ms % 1000) * 1000) };
        ret = Socket_Setsockopt_Func(this->m_sSock, SOL_SOCKET, SO_LINGER, (char*)&lt, sizeof(lt));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool Socket::GetLocalAddr(string &ip, int &port, string &hostName)
    {
        sockaddr_in addr;
        length_var len = sizeof(addr);
        int ret = -1;
        ret = getsockname(this->m_sSock, (sockaddr*)&addr, &len);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        //ip地址
        char szTemp[1024] = { 0 };
        strcpy(szTemp, inet_ntoa(addr.sin_addr));
        ip = szTemp;

        //端口
        port = ntohs(addr.sin_port);

        //主机名
        memset(szTemp, 0, 1024);
        gethostname(szTemp, 1024);
        hostName = szTemp;

        return true;
    }

    bool Socket::GetPeerAddr(string &ip, int &port, string &hostName)
    {
        sockaddr_in addr;
        length_var len = sizeof(addr);
        int ret = -1;
        ret = getpeername(this->m_sSock, (sockaddr*)&addr, &len);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        hostent* remote = nullptr;
        //ip地址
        char szTemp[1024] = { 0 };
        strcpy(szTemp, inet_ntoa(addr.sin_addr));
        ip = szTemp;

        //端口
        port = ntohs(addr.sin_port);
        
        //主机名
        remote = gethostbyname(szTemp);
        memset(szTemp, 0, 1024);
        strcpy(szTemp, remote->h_name);
        hostName = szTemp;

        return true;
    }

    bool Socket::IsCanRead(void)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(this->m_sSock, &readfds);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ret = Socket_Select_Func(1, &readfds, NULL, NULL, &timeout);
        if (ret > 0 && FD_ISSET(this->m_sSock, &readfds)) {
            return true;
        }
        return false;
    }

    bool Socket::IsCanWrite(void)
    {
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(this->m_sSock, &writefds);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int ret = Socket_Select_Func(1, NULL, &writefds, NULL, &timeout);
        if (ret > 0 && FD_ISSET(this->m_sSock, &writefds)) {
            return true;
        }
        return false;
    }

    socket_var Socket::GetSocket(void)
    {
        return this->m_sSock;
    }

    int Socket::ErroCode(void)
    {
        return SOCKET_ERROR_CODE;
    }

    Socket::StatusEnum Socket::Status(void)
    {
#ifdef _WIN32
        switch (SOCKET_ERROR_CODE) {
        case 0:
            return StatusEnum::SE_NORMAL;
        case 10060:
            return StatusEnum::SE_TIMEOUT;
        default:
            return StatusEnum::SE_EXCEPT;
            break;
        }
#endif

#ifdef __linux__
        switch (SOCKET_ERROR_CODE) {
        case 0:
            return StatusEnum::SE_NORMAL;
        case 107:
        case 110:
            return StatusEnum::SE_TIMEOUT;
        default:
            return StatusEnum::SE_EXCEPT;
            break;
        }
#endif

        return StatusEnum::SE_NORMAL;
    }


    //////////////////////////////////////////////////////

    TCPSocket::TCPSocket(void)
    {
#ifdef __linux__
        this->m_nEpollHandle = -1;
#endif
        this->m_bIsSelectMode = false;
        FD_ZERO(&this->m_fdSet);
    }

    TCPSocket::TCPSocket(const TCPSocket &obj)
    {
    }

    TCPSocket& TCPSocket::operator =(const TCPSocket& obj)
    {
        if (this == &obj) {
            return *this;
        }

        return *this;
    }

    TCPSocket::~TCPSocket(void)
    {
        for (auto &client : this->m_mapClient) {
            delete client.second;
        }
        this->m_mapClient.clear();
    }

    bool TCPSocket::acceptClient(Socket* &ptSock)
    {
        sockaddr_in addr = { 0 };
        length_var len = length_var(sizeof(addr));
        socket_var tmp_s = Socket_Accept_Func(this->m_objSock.m_sSock, (sockaddr*)&addr, &len);
        if (INVALID_SOCKET_FLAG == tmp_s) {
            return false;
        }

        Socket* sock = new Socket();
        sock->m_sSock = tmp_s;
        ptSock = sock;
        this->m_mapClient[tmp_s] = sock;
        if (this->m_bIsSelectMode) {
            FD_SET(tmp_s, &this->m_fdSet);
        }
        else {
#ifdef __linux__
            epoll_event evt = { 0 };
            evt.events = EPOLLIN | EPOLLOUT;
            evt.data.fd = tmp_s;

            int ret = -1;
            ret = epoll_ctl(this->m_nEpollHandle, EPOLL_CTL_ADD, tmp_s, &evt);
            if (-1 == ret) {
                return false;
            }
#endif
        }
        return true;
    }

    bool TCPSocket::closeClient(socket_var sock)
    {
        if (INVALID_SOCKET_FLAG == sock) {
            return true;
        }
        int ret = -1;
        ret = Socket_Shutdown_Func(sock, 2);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        ret = Socket_Close_Func(sock);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool TCPSocket::Create(void)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if ((::WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
            return false;
        }
#endif

        this->m_objSock.m_sSock = Socket_Socket_Func(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET_FLAG == this->m_objSock.m_sSock) {
            return false;
        }
        return true;
    }

    bool TCPSocket::Bind(const string &ip, int port, bool isSelect)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        if (ip != "") {
            addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        }
        this->m_bIsSelectMode = true;
#endif
#ifdef __linux__
        if (ip != "") {
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.s_addr = 0;
        }
        this->m_bIsSelectMode = isSelect;
#endif
        int ret = -1;
        ret = Socket_Bind_Func(this->m_objSock.m_sSock, (sockaddr*)&addr, sizeof(addr));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }

        //将服务端套接字加入集合/事件
        if (this->m_bIsSelectMode) {
            FD_SET(this->m_objSock.m_sSock, &this->m_fdSet);
        }
        else {
#ifdef __linux__
            this->m_nEpollHandle = epoll_create(MAX_CLIENT_NUM);
            if (-1 == this->m_nEpollHandle) {
                return false;
            }
            epoll_event evt = { 0 };
            evt.events = EPOLLIN;
            evt.data.fd = this->m_objSock.m_sSock;

            int ret = -1;
            ret = epoll_ctl(this->m_nEpollHandle, EPOLL_CTL_ADD, this->m_objSock.m_sSock, &evt);
            if (-1 == ret) {
                return false;
            }
#endif
        }
        return true;
    }

    bool TCPSocket::Listen()
    {
        int ret = -1;
        ret = Socket_Listen_Func(this->m_objSock.m_sSock, 5);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool TCPSocket::Connect(const string &ip, int port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        if (ip != "") {
            addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        }
#endif
#ifdef __linux__
        if (ip != "") {
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.s_addr = 0;
        }
#endif
        int ret = -1;
        ret = Socket_Connect_Func(this->m_objSock.m_sSock, (sockaddr*)&addr, sizeof(addr));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool TCPSocket::Close(void)
    {
        //先关闭所有客户端
        for (auto &client : this->m_mapClient) {
            this->closeClient(client.first);
            delete client.second;
        }
        this->m_mapClient.clear();
        if (this->m_bIsSelectMode) {
            FD_ZERO(&this->m_fdSet);
        }
        else {
#ifdef __linux__
            this->m_nEpollHandle = -1;
#endif
        }

        if (INVALID_SOCKET_FLAG == this->m_objSock.m_sSock) {
            return true;
        }
        int ret = -1;
        ret = Socket_Shutdown_Func(this->m_objSock.m_sSock, 2);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        ret = Socket_Close_Func(this->m_objSock.m_sSock);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        this->m_objSock.m_sSock = -1;
        return true;
    }

    bool TCPSocket::Select(int ms, FuncDataCallback func)
    {
        if (!this->m_bIsSelectMode) {
            return false;
        }

        fd_set fdRead = this->m_fdSet;
        fd_set fdWrite = this->m_fdSet;
        timeval overTime = { ms / 1000, (ms % 1000) * 1000 };
        int ret = -1;
        ret = Socket_Select_Func(MAX_CLIENT_NUM, &fdRead, &fdWrite, NULL, &overTime);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }

        if (ret >= 0) {
            //新连接
            if (FD_ISSET(this->m_objSock.m_sSock, &fdRead)) {
                Socket* ptClient = nullptr;
                if (this->acceptClient(ptClient)) {
                    func(this, ptClient, 0x00);
                }
            }

            map <socket_var, Socket*> mapExcept;
            for (auto &client : this->m_mapClient) {
                //读
                if (FD_ISSET(client.first, &fdRead)) {
                    func(this, client.second, 0x01);
                    if (Socket::SE_EXCEPT == Socket::Status()) {
                        mapExcept[client.first] = client.second;
                    }
                }

                //写
                if (FD_ISSET(client.first, &fdWrite)) {
                    func(this, client.second, 0x02);
                    if (Socket::SE_EXCEPT == Socket::Status()) {
                        mapExcept[client.first] = client.second;
                    }
                }
            }
            for (auto &it : mapExcept) {
                func(this, it.second, 0x03);
                this->closeClient(it.first);
                FD_CLR(it.first, &this->m_fdSet);
                delete it.second;
                this->m_mapClient.erase(it.first);
            }
        }
        return true;
    }

#ifdef __linux__
    bool TCPSocket::Epoll(int ms, FuncDataCallback func)
    {
        if (this->m_bIsSelectMode) {
            return false;
        }

        epoll_event evts[MAX_CLIENT_NUM] = { 0 };
        int ret = -1;
        ret = epoll_wait(this->m_nEpollHandle, evts, MAX_CLIENT_NUM, ms);
        if (-1 == ret) {
            return false;
        }

        if (ret >= 0) {
            map <socket_var, Socket*> mapExcept;
            for (int i = 0; i < ret; i++) {
                //新连接
                if (evts[i].data.fd == this->m_objSock.m_sSock) {
                    Socket* ptClient = nullptr;
                    if (this->acceptClient(ptClient)) {
                        func(this, ptClient, 0x00);
                    }
                }
                else {
                    //读
                    if (evts[i].events & EPOLLIN) {
                        func(this, this->m_mapClient[evts[i].data.fd], 0x01);
                        if (Socket::SE_EXCEPT == Socket::Status()) {
                            mapExcept[evts[i].data.fd] = this->m_mapClient[evts[i].data.fd];
                        }
                    }

                    //写
                    if (evts[i].events & EPOLLOUT) {
                        func(this, this->m_mapClient[evts[i].data.fd], 0x02);
                        if (Socket::SE_EXCEPT == Socket::Status()) {
                            mapExcept[evts[i].data.fd] = this->m_mapClient[evts[i].data.fd];
                        }
                    }
                }
            }
            for (auto &it : mapExcept) {
                func(this, it.second, 0x03);
                this->closeClient(it.first);
                epoll_ctl(this->m_nEpollHandle, EPOLL_CTL_DEL, it.first, 0);
                delete it.second;
                this->m_mapClient.erase(it.first);
            }
        }
        return true;
    }
#endif

    bool TCPSocket::Send(const char* buf, int len, int flag)
    {
        int ret = Socket_Send_Func(this->m_objSock.m_sSock, buf, len, flag);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool TCPSocket::Recv(char* buf, int &len, int flag)
    {
        int ret = Socket_Recv_Func(this->m_objSock.m_sSock, buf, len, flag);
        len = ret;
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        buf[ret] = '\0';
        return true;
    }

    bool TCPSocket::Send(Socket* ptSock, const char* buf, int len, int flag)
    {
        int ret = Socket_Send_Func(ptSock->m_sSock, buf, len, flag);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }
    
    bool TCPSocket::Recv(Socket* ptSock, char* buf, int &len, int flag)
    {
        int ret = Socket_Recv_Func(ptSock->m_sSock, buf, len, flag);
        len = ret;
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        buf[ret] = '\0';
        return true;
    }

    Socket &TCPSocket::GetSocketObj(void)
    {
        return this->m_objSock;
    }

    const map<socket_var, Socket*> TCPSocket::GetClientMap(void) const
    {
        return this->m_mapClient;
    }

    //////////////////////////////////////////////////////

    UDPSocket::UDPSocket(void)
    {
    }

    UDPSocket::UDPSocket(const UDPSocket &obj)
    {
    }

    UDPSocket& UDPSocket::operator =(const UDPSocket& obj)
    {
        if (this == &obj) {
            return *this;
        }

        return *this;
    }

    UDPSocket::~UDPSocket(void)
    {
    }

    bool UDPSocket::Create(void)
    {
#ifdef _WIN32
        WSADATA wsaData;
        if ((::WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
            return false;
        }
#endif

        this->m_objSock.m_sSock = Socket_Socket_Func(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (INVALID_SOCKET_FLAG == this->m_objSock.m_sSock) {
            return false;
        }
        return true;
    }

    bool UDPSocket::Bind(const string &ip, int port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        if (ip != "") {
            addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        }
#endif
#ifdef __linux__
        if (ip != "") {
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
        }
        else {
            addr.sin_addr.s_addr = 0;
        }
#endif
        int ret = -1;
        ret = Socket_Bind_Func(this->m_objSock.m_sSock, (sockaddr*)&addr, sizeof(addr));
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool UDPSocket::Close(void)
    {
        if (INVALID_SOCKET_FLAG == this->m_objSock.m_sSock) {
            return true;
        }
        int ret = -1;
        ret = Socket_Shutdown_Func(this->m_objSock.m_sSock, 2);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        ret = Socket_Close_Func(this->m_objSock.m_sSock);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        this->m_objSock.m_sSock = -1;
        return true;
    }

    bool UDPSocket::Send(const char* buf, int len, const char* ip, int port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        addr.sin_addr.S_un.S_addr = inet_addr(ip);
#endif
#ifdef __linux__
        addr.sin_addr.s_addr = inet_addr(ip);
#endif
        length_var addr_len = length_var(sizeof(addr));

        int ret = Socket_Sendto_Func(this->m_objSock.m_sSock, buf, len, 0, (sockaddr*)&addr, addr_len);
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        return true;
    }

    bool UDPSocket::Recv(char* buf, int &len, char* ip, int &port)
    {
        sockaddr_in addr;
        length_var addr_len = length_var(sizeof(addr));

        int ret = Socket_Recvfrom_Func(this->m_objSock.m_sSock, buf, len, 0, (sockaddr*)&addr, &addr_len);
        len = ret;
        if (SOCKET_ERROR_FLAG == ret) {
            return false;
        }
        buf[ret] = '\0';
        strcpy(ip, inet_ntoa(addr.sin_addr));
        port = ntohs(addr.sin_port);
        return true;
    }

    Socket &UDPSocket::GetSocketObj(void)
    {
        return this->m_objSock;
    }
}