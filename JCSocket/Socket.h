/**
 * @file		Socket.h
 * @brief		该文件实现套接字 Socket 封装，支持Windows和Linux系统。针对TCP服务端支持Select和Epoll（Linux特有）两种模式。
 * @copyright	Copyright (c) 2020 JackChen
 * @par 修改记录:
 * <table>
 * <tr><th>修改日期    <th>修改人    <th>描述
 * <tr><td>2020-09-02  <td>JackChen  <td>创建第一版
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
     * 仅支持在 TCPSocket/UDPSocket 类中实例化。该类封装了套接字相关属性。
     */
    class Socket
    {
    public:
        /**
         * @brief
         * 类内枚举，Socket状态值
         */
        enum StatusEnum
        {
            ///正常
            SE_NORMAL,

            ///超时
            SE_TIMEOUT,

            ///异常
            SE_EXCEPT
        };

    private:
        friend class TCPSocket;
        friend class UDPSocket;

        ///套接字
        socket_var m_sSock;

    public:
        /**
         * @brief 设置发送缓冲区大小（单位：字节）
         * @param[in]   size    大小
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetSendBufferSize(int size);

        /**
         * @brief 设置接收缓冲区大小（单位：字节）
         * @param[in]   size    大小
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetRecvBufferSize(int size);

        /**
         * @brief 获取发送缓冲区大小（单位：字节）
         * @param[out]  size    大小
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetSendBufferSize(int &size);

        /**
         * @brief 获取接收缓冲区大小（单位：字节）
         * @param[out]  size    大小
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetRecvBufferSize(int &size);

        /**
         * @brief 设置接收超时（单位：毫秒）
         * @param[in]   ms  时间
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetRecvTimeOut(int ms);

        /**
         * @brief 设置发送超时（单位：毫秒）
         * @param[in]   ms  时间
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetSendTimeOut(int ms);

        /**
         * @brief 获取接收超时（单位：毫秒）
         * @param[out]  ms  时间
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetRecvTimeOut(int &ms);

        /**
         * @brief 获取发送超时（单位：毫秒）
         * @param[out]  ms  时间
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetSendTimeOut(int &ms);

        /**
         * @brief 设置阻塞/非阻塞模式（缺省阻塞）
         * @param[in]   bIsBlock    是否阻塞（true是阻塞，false是非阻塞）
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetBlock(bool bIsBlock);		

        /**
         * @brief 设置端口地址是否重用（防止服务端立即重启后绑定地址失败的情况）
         * @param[in]   bIsReuse    是否重用（true是重用，false是不重用）
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetReuseAddr(bool bIsReuse);		

        /**
         * @brief 设置发送是否延时（套接字发送数据会先发送到缓存区，然后让系统的算法自动把数据从缓冲区发送到对端。这里的延时是指系统的算法）
         * @param[in]   isDelay     是否延时（true延时；false不延时，立即将缓冲区的数据发送到对端）
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetSendDelay(bool isDelay);

        /**
         * @brief 设置套接字关闭延时（单位：毫秒）
         * @param[in]  ms  时间
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool SetLinger(int ms);

        /**
         * @brief 根据套接字获取本地地址
         * @param[out]   ip         ip地址
         * @param[out]   port       端口
         * @param[out]   hostName   主机名
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetLocalAddr(string &ip, int &port, string &hostName);

        /**
         * @brief 根据套接字获取对端地址
         * @param[out]   ip         ip地址
         * @param[out]   port       端口
         * @param[out]   hostName   主机名
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool GetPeerAddr(string &ip, int &port, string &hostName);

        /**
         * @brief 套接字是否可读（即是否有数据来）
         * @return  函数执行结果
         * - true   可读
         * - false  不可读
         */
        bool IsCanRead(void);		

        /**
         * @brief 套接字是否可写（即是否可以把数据写入套接字中）
         * @return  函数执行结果
         * - true   可写
         * - false  不可写
         */
        bool IsCanWrite(void);			
        
        /**
         * @brief 获取套接字
         * @return  套接字
         */
        socket_var GetSocket(void);

        /**
         * @brief 获取错误码
         * @return  错误码
         */
        static int ErroCode(void);

        /**
         * @brief 获取套接字状态
         * @return  套接字状态
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
     * 该类封装了TCP协议的套接字，支持Select和Epoll模式。
     */
    class TCPSocket
    {
    private:
        ///数据回调函数类型【服务端】
        typedef void(*FuncDataCallback)(void* lpContext, Socket* ptClientSock, char flag);

    private:
        ///套接字对象【客户端/服务端】
        Socket m_objSock;

        ///客户端套接字映射列表【服务端】
        map<socket_var, Socket*> m_mapClient;
#ifdef __linux__
        ///Epoll事件句柄【服务端】
        int m_nEpollHandle;
#endif
        ///客户端套接字集合【服务端】
        fd_set m_fdSet;

        ///是否为Select模式，缺省为Select模式。true为Select模式，false为Epoll模式【服务端】
        bool m_bIsSelectMode;

    private:
        /**
         * @brief 接收客户端（即等待连接的客户端套接字）【服务端】
         * @param[out]   ptSock     新接入的套接字对象指针
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool acceptClient(Socket* &ptSock);

        /**
         * @brief 关闭套接字（即客户端）【服务端】
         * @param[in]  sock     套接字
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool closeClient(socket_var sock);

    public:
        /**
         * @brief 创建套接字【客户端/服务端】
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Create(void);

        /**
         * @brief 绑定地址【服务端】
         * @param[in]  ip           ip地址
         * @param[in]  port         端口（一般 > 1024）
         * @param[in]  isSelect     是否为Select模式（缺省true，true为Select模式，false为Epoll模式）
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Bind(const string &ip, int port, bool isSelect = true);

        /**
         * @brief 监听【服务端】
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Listen(void);

        /**
         * @brief 连接（即服务端）【客户端】
         * @param[in]  ip       ip地址
         * @param[in]  port     端口
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Connect(const string &ip, int port);

        /**
         * @brief 关闭套接字【客户端/服务端】
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Close(void);

        /**
         * @brief Select轮询模式【服务端】
         * @param[in]  ms       超时，只有在非阻塞模式下，该值才生效（单位：毫秒）
         * @param[in]  func     数据处理回调函数指针
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Select(int ms, FuncDataCallback func);
#ifdef __linux__
        /**
         * @brief Epoll轮询模式【服务端】
         * @param[in]  ms       超时，只有在非阻塞模式下，该值才生效（单位：毫秒）
         * @param[in]  func     数据处理回调函数指针
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Epoll(int ms, FuncDataCallback func);
#endif
        /**
         * @brief 发送数据【客户端/服务端】
         * @param[in]  buf      数据内容
         * @param[in]  len      数据长度
         * @param[in]  flag     系统send的flag值
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Send(const char* buf, int len, int flag = 0);

        /**
         * @brief 接收数据【客户端/服务端】
         * @param[out]  buf      数据接收地址
         * @param[out]  len      数据长度
         * @param[in]   flag     原recv的flag值
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Recv(char* buf, int &len, int flag = 0);

        /**
         * @brief 发送数据到指定套接字【客户端/服务端】
         * @param[in]  ptSock   套接字对象指针
         * @param[in]  buf      数据内容
         * @param[in]  len      数据长度
         * @param[in]  flag     系统send的flag值
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        static bool Send(Socket* ptSock, const char* buf, int len, int flag = 0);

        /**
         * @brief 从指定套接字接收数据【客户端/服务端】
         * @param[in]   ptSock   套接字对象指针
         * @param[out]  buf      数据接收地址
         * @param[out]  len      数据长度
         * @param[in]   flag     原recv的flag值
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        static bool Recv(Socket* ptSock, char* buf, int &len, int flag = 0);

        /**
         * @brief 获取套接字对象【客户端/服务端】
         * @return  套接字对象
         */
        Socket &GetSocketObj(void);

        /**
         * @brief 获取客户端套接字映射列表【服务端】
         * @return  客户端套接字映射列表
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
     * 该类封装了UDP协议的套接字。
     */
    class UDPSocket
    {
    private:
        ///套接字对象【客户端/服务端】
        Socket m_objSock;

    public:
        /**
         * @brief 创建套接字【客户端/服务端】
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Create(void);

        /**
         * @brief 绑定地址【服务端】
         * @param[in]  ip           ip地址
         * @param[in]  port         端口（一般 > 1024）
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Bind(const string &ip, int port);

        /**
         * @brief 关闭套接字【客户端/服务端】
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Close(void);

        /**
         * @brief 发送数据【客户端/服务端】
         * @param[in]  buf      数据内容
         * @param[in]  len      数据长度
         * @param[in]  ip       发送地址
         * @param[in]  port     发送端口
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Send(const char* buf, int len, const char* ip, int port);

        /**
         * @brief 接收数据【客户端/服务端】
         * @param[out]  buf     数据接收地址
         * @param[out]  len     数据长度
         * @param[out]  ip      数据来源的ip地址
         * @param[out]  port    数据来源的端口
         * @return  函数执行结果
         * - true   成功
         * - false  失败
         */
        bool Recv(char* buf, int &len, char* ip, int &port);

        /**
         * @brief 获取套接字对象【客户端/服务端】
         * @return  套接字对象
         */
        Socket &GetSocketObj(void);

    public:
        UDPSocket(void);
        UDPSocket(const UDPSocket &obj);
        UDPSocket& operator =(const UDPSocket& obj);
        virtual ~UDPSocket(void);
    };
}