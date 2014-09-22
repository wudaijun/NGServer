#ifndef __NGSERVER_SESSION_H_INCLUDE__
#define __NGSERVER_SESSION_H_INCLUDE__
#include "NetHead.h"
#include "MessageQueue.h"

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

private:
    // 异步读取数据
    void AsyncReadSome();
    // 读取完成：处理数据并继续读取
    void ReadComplete(const boost::system::error_code& err, size_t bytes_transferred);
    // 处理接收缓冲区的数据
    void ProcessData();

    // 发送数据
    void SendData(const char* data, size_t len);
    // 发送完成
    void SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred);

private:
    std::shared_ptr<Socket> _socket;
    uint32_t _id;

    bool _run = false;

    char _recv_buf[kBufferSize];
    size_t _recv_off = 0;
    size_t _recv_total = 0;

    ByteBuff _send_buf;
    size_t _send_total = 0;

    Locker _sending_lock;
};

#endif