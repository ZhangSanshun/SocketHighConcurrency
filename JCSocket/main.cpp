/**
 * @file		main.cpp
 * @brief		案例测试（linux上gcc需要4.9+版本）。
 * @copyright	Copyright (c) 2020 JackChen
 * @par 修改记录:
 * <table>
 * <tr><th>修改日期    <th>修改人    <th>描述
 * <tr><td>2020-09-02  <td>JackChen  <td>创建第一版
 * </table>
 */
#ifdef __linux__
#include <signal.h>
#endif
#include "Socket.h"

using namespace jc;


///TCP协议服务端（非阻塞模式 Select）
#define TEST_1

///TCP协议客户端（非阻塞模式）
//#define TEST_2

///UDP协议服务端（非阻塞模式）
//#define TEST_3

///UDP协议客户端（非阻塞模式）
//#define TEST_4

///TCP协议服务端（非阻塞模式 epoll（Linux特有））
//#define TEST_5


#ifdef TEST_1

void DataProc(void* lpContext, Socket* ptClientSock, char flag)
{
    TCPSocket* lpThis = (TCPSocket*)lpContext;
    
    switch (flag) {
    case 0x00: {
        //新连接
        printf("new client\n");
        ptClientSock->SetBlock(false);
        ptClientSock->SetSendTimeOut(10);
        ptClientSock->SetRecvTimeOut(10);
        break;
    }
    case 0x01: {
        //有数据可读
        char szTemp[1024] = { 0 };
        int len = 1024;
        if (TCPSocket::Recv(ptClientSock, szTemp, len)) {
            printf("%d: %s\n", len, szTemp);
        }
        break;
    }
    case 0x02: {
        //可以写入数据
        char szBuffer[] = "欢迎关注微信公众号：程序员JC\n";
        if (!TCPSocket::Send(ptClientSock, szBuffer, strlen(szBuffer))) {
            if (Socket::SE_TIMEOUT == Socket::Status()) {
                printf("发送超时\n");
            }
        }
        break;
    }
    case 0x03: {
        //关闭连接
        printf("del client\n");
        break;
    }
    default:
        break;
    }
}

int main(void)
{
#ifdef __linux__
    //linux下进程可能会突然中止（屏蔽突然中断信号）
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPServer;
    //1）创建
    objTCPServer.Create();

    //设置非阻塞模式（发送超时、接收超时）
    Socket &objSock = objTCPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2）绑定
    objTCPServer.Bind("127.0.0.1", 6789);

    //3）监听
    objTCPServer.Listen();

    while (true) {
        //4）轮询
        objTCPServer.Select(10, DataProc);
        printf("client num: %d\n", objTCPServer.GetClientMap().size());
#ifdef _WIN32
        Sleep(1000);
#endif
#ifdef __linux__
        sleep(1);
#endif
    }
    objTCPServer.Close();
    return 0;
}

#elif defined TEST_2

int main(void)
{
#ifdef __linux__
    //linux下进程可能会突然中止（屏蔽突然中断信号）
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPClient;
    //1）创建
    objTCPClient.Create();

    //设置非阻塞模式（发送超时、接收超时）
    Socket &objSock = objTCPClient.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2）连接
    objTCPClient.Connect("127.0.0.1", 6789);

    while (true) {
        if (objSock.IsCanWrite()) {
            char szBuffer[] = "欢迎关注微信公众号：程序员JC\n";
            //3）发送
            if (!objTCPClient.Send(szBuffer, strlen(szBuffer))) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("发送超时\n");
                }
            }
        }

        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            //3）接收
            if (!objTCPClient.Recv(szTemp, len)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("接收超时\n");
                }
                else {
                    printf("接收失败 %d\n", Socket::ErroCode());
                }
            }
            printf("%d: %s\n", len, szTemp);
        }

#ifdef _WIN32
        Sleep(1000);
#endif
#ifdef __linux__
        sleep(1);
#endif
    }
    objTCPClient.Close();
    return 0;
}

#elif defined TEST_3

int main(void)
{
#ifdef __linux__
    //linux下进程可能会突然中止（屏蔽突然中断信号）
    signal(SIGPIPE, SIG_IGN);
#endif

    UDPSocket objUDPServer;
    //1）创建
    objUDPServer.Create();

    //设置非阻塞模式（发送超时、接收超时）
    Socket &objSock = objUDPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2）绑定
    objUDPServer.Bind("127.0.0.1", 6789);

    //先从客户端发送一个数据包到服务端，获取客户端地址
    char ip[100] = { 0 };
    int port = 0;
    while (true) {
        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            //3）接收
            if (!objUDPServer.Recv(szTemp, len, ip, port)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("接收超时\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("发送异常 %d\n", Socket::ErroCode());
                    memset(ip, 0, 100);
                    port = 0;
                }
            }
            else {
                printf("[%s:%d] - %d: %s\n", ip, port, len, szTemp);
            }
        }

        if (port != 0) {
            if (objSock.IsCanWrite()) {
                //3）发送
                char szBuffer[] = "欢迎关注微信公众号：程序员JC\n";
                if (!objUDPServer.Send(szBuffer, strlen(szBuffer), ip, port)) {
                    if (Socket::SE_TIMEOUT == Socket::Status()) {
                        printf("发送超时\n");
                    }
                    else if (Socket::SE_EXCEPT == Socket::Status()) {
                        printf("发送异常\n");
                    }
                }
            }
        }

#ifdef _WIN32
        Sleep(1000);
#endif
#ifdef __linux__
        sleep(1);
#endif
    }
    objUDPServer.Close();
    return 0;
}

#elif defined TEST_4

int main(void)
{
#ifdef __linux__
    //linux下进程可能会突然中止（屏蔽突然中断信号）
    signal(SIGPIPE, SIG_IGN);
#endif

    UDPSocket objUDPClient;
    //1）创建
    objUDPClient.Create();

    //设置非阻塞模式（发送超时、接收超时）
    Socket &objSock = objUDPClient.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    while (true) {
        if (objSock.IsCanWrite()) {
            //2）发送
            char szBuffer[] = "欢迎关注微信公众号：程序员JC\n";
            if (!objUDPClient.Send(szBuffer, strlen(szBuffer), "127.0.0.1", 6789)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("发送超时\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("发送失败\n");
                }
            }
        }

        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            char ip[100] = { 0 };
            int port = 0;
            //2）接收
            if (!objUDPClient.Recv(szTemp, len, ip, port)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("接收超时\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("接收失败 %d\n", Socket::ErroCode());
                }
            }
            else {
                printf("[%s:%d] - %d: %s\n", ip, port, len, szTemp);
            }
        }

#ifdef _WIN32
        Sleep(1000);
#endif
#ifdef __linux__
        sleep(1);
#endif
    }
    objUDPClient.Close();
    return 0;
}

#elif defined TEST_5

#ifdef __linux__

void DataProc(void* lpContext, Socket* ptClientSock, char flag)
{
    TCPSocket* lpThis = (TCPSocket*)lpContext;

    switch (flag) {
    case 0x00: {
        //新连接
        printf("new client\n");
        ptClientSock->SetBlock(false);
        ptClientSock->SetSendTimeOut(10);
        ptClientSock->SetRecvTimeOut(10);
        break;
    }
    case 0x01: {
        //有数据可读
        char szTemp[1024] = { 0 };
        int len = 1024;
        if (TCPSocket::Recv(ptClientSock, szTemp, len)) {
            printf("%d: %s\n", len, szTemp);
        }
        break;
    }
    case 0x02: {
        //可以写入数据
        char szBuffer[] = "欢迎关注微信公众号：程序员JC\n";
        if (!TCPSocket::Send(ptClientSock, szBuffer, strlen(szBuffer))) {
            if (Socket::SE_TIMEOUT == Socket::Status()) {
                printf("发送超时\n");
            }
        }
        break;
    }
    case 0x03: {
        //关闭连接
        printf("del client\n");
        break;
    }
    default:
        break;
    }
}

int main(void)
{
#ifdef __linux__
    //linux下进程可能会突然中止（屏蔽突然中断信号）
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPServer;
    //1）创建
    objTCPServer.Create();

    //设置非阻塞模式（发送超时、接收超时）
    Socket &objSock = objTCPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2）绑定
    objTCPServer.Bind("127.0.0.1", 6789, false);

    //3）监听
    objTCPServer.Listen();

    while (true) {
        //4）轮询
        objTCPServer.Epoll(10, DataProc);
        printf("client num: %d\n", objTCPServer.GetClientMap().size());
#ifdef _WIN32
        Sleep(1000);
#endif
#ifdef __linux__
        sleep(1);
#endif
    }
    objTCPServer.Close();
    return 0;
}

#endif

#else

int main(void)
{
    return 0;
}

#endif