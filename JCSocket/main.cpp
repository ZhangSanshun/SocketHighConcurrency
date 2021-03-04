/**
 * @file		main.cpp
 * @brief		�������ԣ�linux��gcc��Ҫ4.9+�汾����
 * @copyright	Copyright (c) 2020 JackChen
 * @par �޸ļ�¼:
 * <table>
 * <tr><th>�޸�����    <th>�޸���    <th>����
 * <tr><td>2020-09-02  <td>JackChen  <td>������һ��
 * </table>
 */
#ifdef __linux__
#include <signal.h>
#endif
#include "Socket.h"

using namespace jc;


///TCPЭ�����ˣ�������ģʽ Select��
#define TEST_1

///TCPЭ��ͻ��ˣ�������ģʽ��
//#define TEST_2

///UDPЭ�����ˣ�������ģʽ��
//#define TEST_3

///UDPЭ��ͻ��ˣ�������ģʽ��
//#define TEST_4

///TCPЭ�����ˣ�������ģʽ epoll��Linux���У���
//#define TEST_5


#ifdef TEST_1

void DataProc(void* lpContext, Socket* ptClientSock, char flag)
{
    TCPSocket* lpThis = (TCPSocket*)lpContext;
    
    switch (flag) {
    case 0x00: {
        //������
        printf("new client\n");
        ptClientSock->SetBlock(false);
        ptClientSock->SetSendTimeOut(10);
        ptClientSock->SetRecvTimeOut(10);
        break;
    }
    case 0x01: {
        //�����ݿɶ�
        char szTemp[1024] = { 0 };
        int len = 1024;
        if (TCPSocket::Recv(ptClientSock, szTemp, len)) {
            printf("%d: %s\n", len, szTemp);
        }
        break;
    }
    case 0x02: {
        //����д������
        char szBuffer[] = "��ӭ��ע΢�Ź��ںţ�����ԱJC\n";
        if (!TCPSocket::Send(ptClientSock, szBuffer, strlen(szBuffer))) {
            if (Socket::SE_TIMEOUT == Socket::Status()) {
                printf("���ͳ�ʱ\n");
            }
        }
        break;
    }
    case 0x03: {
        //�ر�����
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
    //linux�½��̿��ܻ�ͻȻ��ֹ������ͻȻ�ж��źţ�
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPServer;
    //1������
    objTCPServer.Create();

    //���÷�����ģʽ�����ͳ�ʱ�����ճ�ʱ��
    Socket &objSock = objTCPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2����
    objTCPServer.Bind("127.0.0.1", 6789);

    //3������
    objTCPServer.Listen();

    while (true) {
        //4����ѯ
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
    //linux�½��̿��ܻ�ͻȻ��ֹ������ͻȻ�ж��źţ�
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPClient;
    //1������
    objTCPClient.Create();

    //���÷�����ģʽ�����ͳ�ʱ�����ճ�ʱ��
    Socket &objSock = objTCPClient.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2������
    objTCPClient.Connect("127.0.0.1", 6789);

    while (true) {
        if (objSock.IsCanWrite()) {
            char szBuffer[] = "��ӭ��ע΢�Ź��ںţ�����ԱJC\n";
            //3������
            if (!objTCPClient.Send(szBuffer, strlen(szBuffer))) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("���ͳ�ʱ\n");
                }
            }
        }

        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            //3������
            if (!objTCPClient.Recv(szTemp, len)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("���ճ�ʱ\n");
                }
                else {
                    printf("����ʧ�� %d\n", Socket::ErroCode());
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
    //linux�½��̿��ܻ�ͻȻ��ֹ������ͻȻ�ж��źţ�
    signal(SIGPIPE, SIG_IGN);
#endif

    UDPSocket objUDPServer;
    //1������
    objUDPServer.Create();

    //���÷�����ģʽ�����ͳ�ʱ�����ճ�ʱ��
    Socket &objSock = objUDPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2����
    objUDPServer.Bind("127.0.0.1", 6789);

    //�ȴӿͻ��˷���һ�����ݰ�������ˣ���ȡ�ͻ��˵�ַ
    char ip[100] = { 0 };
    int port = 0;
    while (true) {
        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            //3������
            if (!objUDPServer.Recv(szTemp, len, ip, port)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("���ճ�ʱ\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("�����쳣 %d\n", Socket::ErroCode());
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
                //3������
                char szBuffer[] = "��ӭ��ע΢�Ź��ںţ�����ԱJC\n";
                if (!objUDPServer.Send(szBuffer, strlen(szBuffer), ip, port)) {
                    if (Socket::SE_TIMEOUT == Socket::Status()) {
                        printf("���ͳ�ʱ\n");
                    }
                    else if (Socket::SE_EXCEPT == Socket::Status()) {
                        printf("�����쳣\n");
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
    //linux�½��̿��ܻ�ͻȻ��ֹ������ͻȻ�ж��źţ�
    signal(SIGPIPE, SIG_IGN);
#endif

    UDPSocket objUDPClient;
    //1������
    objUDPClient.Create();

    //���÷�����ģʽ�����ͳ�ʱ�����ճ�ʱ��
    Socket &objSock = objUDPClient.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    while (true) {
        if (objSock.IsCanWrite()) {
            //2������
            char szBuffer[] = "��ӭ��ע΢�Ź��ںţ�����ԱJC\n";
            if (!objUDPClient.Send(szBuffer, strlen(szBuffer), "127.0.0.1", 6789)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("���ͳ�ʱ\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("����ʧ��\n");
                }
            }
        }

        if (objSock.IsCanRead()) {
            char szTemp[1024] = { 0 };
            int len = 1024;
            char ip[100] = { 0 };
            int port = 0;
            //2������
            if (!objUDPClient.Recv(szTemp, len, ip, port)) {
                if (Socket::SE_TIMEOUT == Socket::Status()) {
                    printf("���ճ�ʱ\n");
                }
                else if (Socket::SE_EXCEPT == Socket::Status()) {
                    printf("����ʧ�� %d\n", Socket::ErroCode());
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
        //������
        printf("new client\n");
        ptClientSock->SetBlock(false);
        ptClientSock->SetSendTimeOut(10);
        ptClientSock->SetRecvTimeOut(10);
        break;
    }
    case 0x01: {
        //�����ݿɶ�
        char szTemp[1024] = { 0 };
        int len = 1024;
        if (TCPSocket::Recv(ptClientSock, szTemp, len)) {
            printf("%d: %s\n", len, szTemp);
        }
        break;
    }
    case 0x02: {
        //����д������
        char szBuffer[] = "��ӭ��ע΢�Ź��ںţ�����ԱJC\n";
        if (!TCPSocket::Send(ptClientSock, szBuffer, strlen(szBuffer))) {
            if (Socket::SE_TIMEOUT == Socket::Status()) {
                printf("���ͳ�ʱ\n");
            }
        }
        break;
    }
    case 0x03: {
        //�ر�����
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
    //linux�½��̿��ܻ�ͻȻ��ֹ������ͻȻ�ж��źţ�
    signal(SIGPIPE, SIG_IGN);
#endif

    TCPSocket objTCPServer;
    //1������
    objTCPServer.Create();

    //���÷�����ģʽ�����ͳ�ʱ�����ճ�ʱ��
    Socket &objSock = objTCPServer.GetSocketObj();
    objSock.SetBlock(false);
    objSock.SetSendTimeOut(10);
    objSock.SetRecvTimeOut(10);

    //2����
    objTCPServer.Bind("127.0.0.1", 6789, false);

    //3������
    objTCPServer.Listen();

    while (true) {
        //4����ѯ
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