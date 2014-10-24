#ifndef __NGSERVER_SESSION_H_INCLUDE__
#define __NGSERVER_SESSION_H_INCLUDE__
#include "NetHead.h"
#include "MessageQueue.h"

/*
*   文件： Session.h
*   功能： 定义Session类
*   Session类：
*       主要提供三个接口：
*           StartRecv() 启动会话
*           SendAsync() 异步发送数据 内部使用双缓冲提升效率
*           ProcessData() 处理_recv_buff中的数据接口 一般做解码
*/

class Session : public std::enable_shared_from_this<Session>
{
    static const int32_t kBufferSize = 65536; // 64 k 接收区缓冲

public:
    Session(const std::shared_ptr<Socket> socket, int32_t conn_id)
        : _socket(socket), _id(conn_id), _send_buf(4*kBufferSize){}

    // 启动会话
    void StartRecv();

    // 发送数据
    bool SendAsync(const char* data, size_t len);

    void SetDecoder(std::function<int32_t(const char* data, size_t len)>& decoder)
    {
        _decoder = decoder;
    }

    void DisConnect();
private:
    // 异步读取数据
    void AsyncReadSome();
    // 读取完成：处理数据并继续读取
    void ReadComplete(const boost::system::error_code& err, size_t bytes_transferred);
    // 处理接收缓冲区的数据
    void ProcessData();

    // 发送数据 同一时刻只有一个线程执行该函数发送数据
    void SendData(const char* data, size_t len);
    // 发送完成
    void SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred);

private:
    std::function<int32_t(const char* data, size_t len)> _decoder;

    std::shared_ptr<Socket> _socket;
    uint32_t _id;   // 会话ID

    bool _run = false; // 状态标识

    // 接收缓冲区
    char _recv_buf[kBufferSize];
    size_t _recv_off = 0;
    size_t _recv_total = 0;

    // 发送缓冲区(双缓冲)
    ByteBuff _send_buf;
    size_t _send_total = 0;

    // 发送锁 同一时间只能有一个线程发送数据
    Locker _sending_lock;
    Locker _closing_lock;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif