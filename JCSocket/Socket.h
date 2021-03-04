/**
 * @file		Socket.h
 * @brief		���ļ�ʵ���׽��� Socket ��װ��֧��Windows��Linuxϵͳ�����TCP�����֧��Select��Epoll��Linux���У�����ģʽ��
 * @copyright	Copyright (c) 2020 JackChen
 * @par �޸ļ�¼:
 * <table>
 * <tr><th>�޸�����    <th>�޸���    <th>����
 * <tr><td>2020-09-02  <td>JackChen  <td>������һ��
 * </table>
 */
#pragma once

#include <string>
#include <map>
#ifdef _WIN32
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")
#endif
#ifdef __linux__
#include <string.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

using namespace std;


#ifdef _WIN32
typedef SOCKET socket_var;
typedef int length_var;
#endif

#ifdef __linux__
typedef int socket_var;
typedef socklen_t length_var;
#endif


namespace jc
{
    class TCPSocket;
    class UDPSocket;

    /**
     * @brief
     * ��֧���� TCPSocket/UDPSocket ����ʵ�����������װ���׽���������ԡ�
     */
    class Socket
    {
    public:
        /**
         * @brief
         * ����ö�٣�Socket״ֵ̬
         */
        enum StatusEnum
        {
            ///����
            SE_NORMAL,

            ///��ʱ
            SE_TIMEOUT,

            ///�쳣
            SE_EXCEPT
        };

    private:
        friend class TCPSocket;
        friend class UDPSocket;

        ///�׽���
        socket_var m_sSock;

    public:
        /**
         * @brief ���÷��ͻ�������С����λ���ֽڣ�
         * @param[in]   size    ��С
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetSendBufferSize(int size);

        /**
         * @brief ���ý��ջ�������С����λ���ֽڣ�
         * @param[in]   size    ��С
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetRecvBufferSize(int size);

        /**
         * @brief ��ȡ���ͻ�������С����λ���ֽڣ�
         * @param[out]  size    ��С
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetSendBufferSize(int &size);

        /**
         * @brief ��ȡ���ջ�������С����λ���ֽڣ�
         * @param[out]  size    ��С
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetRecvBufferSize(int &size);

        /**
         * @brief ���ý��ճ�ʱ����λ�����룩
         * @param[in]   ms  ʱ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetRecvTimeOut(int ms);

        /**
         * @brief ���÷��ͳ�ʱ����λ�����룩
         * @param[in]   ms  ʱ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetSendTimeOut(int ms);

        /**
         * @brief ��ȡ���ճ�ʱ����λ�����룩
         * @param[out]  ms  ʱ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetRecvTimeOut(int &ms);

        /**
         * @brief ��ȡ���ͳ�ʱ����λ�����룩
         * @param[out]  ms  ʱ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetSendTimeOut(int &ms);

        /**
         * @brief ��������/������ģʽ��ȱʡ������
         * @param[in]   bIsBlock    �Ƿ�������true��������false�Ƿ�������
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetBlock(bool bIsBlock);		

        /**
         * @brief ���ö˿ڵ�ַ�Ƿ����ã���ֹ���������������󶨵�ַʧ�ܵ������
         * @param[in]   bIsReuse    �Ƿ����ã�true�����ã�false�ǲ����ã�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetReuseAddr(bool bIsReuse);		

        /**
         * @brief ���÷����Ƿ���ʱ���׽��ַ������ݻ��ȷ��͵���������Ȼ����ϵͳ���㷨�Զ������ݴӻ��������͵��Զˡ��������ʱ��ָϵͳ���㷨��
         * @param[in]   isDelay     �Ƿ���ʱ��true��ʱ��false����ʱ�������������������ݷ��͵��Զˣ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetSendDelay(bool isDelay);

        /**
         * @brief �����׽��ֹر���ʱ����λ�����룩
         * @param[in]  ms  ʱ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool SetLinger(int ms);

        /**
         * @brief �����׽��ֻ�ȡ���ص�ַ
         * @param[out]   ip         ip��ַ
         * @param[out]   port       �˿�
         * @param[out]   hostName   ������
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetLocalAddr(string &ip, int &port, string &hostName);

        /**
         * @brief �����׽��ֻ�ȡ�Զ˵�ַ
         * @param[out]   ip         ip��ַ
         * @param[out]   port       �˿�
         * @param[out]   hostName   ������
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool GetPeerAddr(string &ip, int &port, string &hostName);

        /**
         * @brief �׽����Ƿ�ɶ������Ƿ�����������
         * @return  ����ִ�н��
         * - true   �ɶ�
         * - false  ���ɶ�
         */
        bool IsCanRead(void);		

        /**
         * @brief �׽����Ƿ��д�����Ƿ���԰�����д���׽����У�
         * @return  ����ִ�н��
         * - true   ��д
         * - false  ����д
         */
        bool IsCanWrite(void);			
        
        /**
         * @brief ��ȡ�׽���
         * @return  �׽���
         */
        socket_var GetSocket(void);

        /**
         * @brief ��ȡ������
         * @return  ������
         */
        static int ErroCode(void);

        /**
         * @brief ��ȡ�׽���״̬
         * @return  �׽���״̬
         */
        static StatusEnum Status(void);

    private:
        Socket(void);
        Socket(const Socket &obj);
        Socket& operator =(const Socket& obj);
        virtual ~Socket(void);
    };


    /**
     * @brief
     * �����װ��TCPЭ����׽��֣�֧��Select��Epollģʽ��
     */
    class TCPSocket
    {
    private:
        ///���ݻص��������͡�����ˡ�
        typedef void(*FuncDataCallback)(void* lpContext, Socket* ptClientSock, char flag);

    private:
        ///�׽��ֶ��󡾿ͻ���/����ˡ�
        Socket m_objSock;

        ///�ͻ����׽���ӳ���б�����ˡ�
        map<socket_var, Socket*> m_mapClient;
#ifdef __linux__
        ///Epoll�¼����������ˡ�
        int m_nEpollHandle;
#endif
        ///�ͻ����׽��ּ��ϡ�����ˡ�
        fd_set m_fdSet;

        ///�Ƿ�ΪSelectģʽ��ȱʡΪSelectģʽ��trueΪSelectģʽ��falseΪEpollģʽ������ˡ�
        bool m_bIsSelectMode;

    private:
        /**
         * @brief ���տͻ��ˣ����ȴ����ӵĿͻ����׽��֣�������ˡ�
         * @param[out]   ptSock     �½�����׽��ֶ���ָ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool acceptClient(Socket* &ptSock);

        /**
         * @brief �ر��׽��֣����ͻ��ˣ�������ˡ�
         * @param[in]  sock     �׽���
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool closeClient(socket_var sock);

    public:
        /**
         * @brief �����׽��֡��ͻ���/����ˡ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Create(void);

        /**
         * @brief �󶨵�ַ������ˡ�
         * @param[in]  ip           ip��ַ
         * @param[in]  port         �˿ڣ�һ�� > 1024��
         * @param[in]  isSelect     �Ƿ�ΪSelectģʽ��ȱʡtrue��trueΪSelectģʽ��falseΪEpollģʽ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Bind(const string &ip, int port, bool isSelect = true);

        /**
         * @brief ����������ˡ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Listen(void);

        /**
         * @brief ���ӣ�������ˣ����ͻ��ˡ�
         * @param[in]  ip       ip��ַ
         * @param[in]  port     �˿�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Connect(const string &ip, int port);

        /**
         * @brief �ر��׽��֡��ͻ���/����ˡ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Close(void);

        /**
         * @brief Select��ѯģʽ������ˡ�
         * @param[in]  ms       ��ʱ��ֻ���ڷ�����ģʽ�£���ֵ����Ч����λ�����룩
         * @param[in]  func     ���ݴ���ص�����ָ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Select(int ms, FuncDataCallback func);
#ifdef __linux__
        /**
         * @brief Epoll��ѯģʽ������ˡ�
         * @param[in]  ms       ��ʱ��ֻ���ڷ�����ģʽ�£���ֵ����Ч����λ�����룩
         * @param[in]  func     ���ݴ���ص�����ָ��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Epoll(int ms, FuncDataCallback func);
#endif
        /**
         * @brief �������ݡ��ͻ���/����ˡ�
         * @param[in]  buf      ��������
         * @param[in]  len      ���ݳ���
         * @param[in]  flag     ϵͳsend��flagֵ
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Send(const char* buf, int len, int flag = 0);

        /**
         * @brief �������ݡ��ͻ���/����ˡ�
         * @param[out]  buf      ���ݽ��յ�ַ
         * @param[out]  len      ���ݳ���
         * @param[in]   flag     ԭrecv��flagֵ
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Recv(char* buf, int &len, int flag = 0);

        /**
         * @brief �������ݵ�ָ���׽��֡��ͻ���/����ˡ�
         * @param[in]  ptSock   �׽��ֶ���ָ��
         * @param[in]  buf      ��������
         * @param[in]  len      ���ݳ���
         * @param[in]  flag     ϵͳsend��flagֵ
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        static bool Send(Socket* ptSock, const char* buf, int len, int flag = 0);

        /**
         * @brief ��ָ���׽��ֽ������ݡ��ͻ���/����ˡ�
         * @param[in]   ptSock   �׽��ֶ���ָ��
         * @param[out]  buf      ���ݽ��յ�ַ
         * @param[out]  len      ���ݳ���
         * @param[in]   flag     ԭrecv��flagֵ
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        static bool Recv(Socket* ptSock, char* buf, int &len, int flag = 0);

        /**
         * @brief ��ȡ�׽��ֶ��󡾿ͻ���/����ˡ�
         * @return  �׽��ֶ���
         */
        Socket &GetSocketObj(void);

        /**
         * @brief ��ȡ�ͻ����׽���ӳ���б�����ˡ�
         * @return  �ͻ����׽���ӳ���б�
         */
        const map<socket_var, Socket*> GetClientMap(void) const;

    public:
        TCPSocket(void);
        TCPSocket(const TCPSocket &obj);
        TCPSocket& operator =(const TCPSocket& obj);
        virtual ~TCPSocket(void);
    };


    /**
     * @brief
     * �����װ��UDPЭ����׽��֡�
     */
    class UDPSocket
    {
    private:
        ///�׽��ֶ��󡾿ͻ���/����ˡ�
        Socket m_objSock;

    public:
        /**
         * @brief �����׽��֡��ͻ���/����ˡ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Create(void);

        /**
         * @brief �󶨵�ַ������ˡ�
         * @param[in]  ip           ip��ַ
         * @param[in]  port         �˿ڣ�һ�� > 1024��
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Bind(const string &ip, int port);

        /**
         * @brief �ر��׽��֡��ͻ���/����ˡ�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Close(void);

        /**
         * @brief �������ݡ��ͻ���/����ˡ�
         * @param[in]  buf      ��������
         * @param[in]  len      ���ݳ���
         * @param[in]  ip       ���͵�ַ
         * @param[in]  port     ���Ͷ˿�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Send(const char* buf, int len, const char* ip, int port);

        /**
         * @brief �������ݡ��ͻ���/����ˡ�
         * @param[out]  buf     ���ݽ��յ�ַ
         * @param[out]  len     ���ݳ���
         * @param[out]  ip      ������Դ��ip��ַ
         * @param[out]  port    ������Դ�Ķ˿�
         * @return  ����ִ�н��
         * - true   �ɹ�
         * - false  ʧ��
         */
        bool Recv(char* buf, int &len, char* ip, int &port);

        /**
         * @brief ��ȡ�׽��ֶ��󡾿ͻ���/����ˡ�
         * @return  �׽��ֶ���
         */
        Socket &GetSocketObj(void);

    public:
        UDPSocket(void);
        UDPSocket(const UDPSocket &obj);
        UDPSocket& operator =(const UDPSocket& obj);
        virtual ~UDPSocket(void);
    };
}