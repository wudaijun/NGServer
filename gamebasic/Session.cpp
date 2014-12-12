#include "Session.h"

void Session::StartRecv()
{
    if (_runing_lock.TryLock())
        AsyncReadSome();
}

void Session::AsyncReadSome()
{
    if (_socket)
        _socket->async_read_some(
            boost::asio::buffer(_recv_buf + _recv_off, kBufferSize - _recv_off),
            boost::bind(&Session::ReadComplete, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void Session::ReadComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
    {
        DisConnect();
        return;
    }
    _recv_total += bytes_transferred;
    _recv_off += bytes_transferred;

    // 处理数据
    if (ProcessData())
    {
        // 继续接收数据
        AsyncReadSome();
    }
}

// 对缓冲区中的数据解包 返回false则断开连接
bool Session::ProcessData()
{
    if (_decoder == nullptr)
        return false;

    // 将数据交由解码器处理 返回处理之后的缓冲区剩余字节数 返回-1表示服务器主动断线
    int32_t remain = _decoder(_recv_buf,_recv_off);

    // 服务器断开连接
    if (remain < 0)
    {
        DisConnect();
        return false;
    }

    // 处理之后的偏移
    if (remain > 0 && remain < kBufferSize)
    {
        size_t remain_off = _recv_off - remain;
        _recv_off = (size_t)remain;
        memcpy(_recv_buf, _recv_buf + remain_off, _recv_off);
    }
    else
    {
        _recv_off = 0;
    }
    return true;
}

// 发送数据
bool Session::SendAsync(const char* data, size_t len)
{
    if (_socket == nullptr)
        return false;

    if (_send_buf.Push(data, len))
    {
        if (_sending_lock.TryLock())
        {
            size_t sendlen;
            const char* data = _send_buf.PopAll(sendlen);
            SendData(data, sendlen);
            return true;
        }
    }
    else
    {
        assert(0); // 发送缓冲区满
        return false;
    }
}

// 发送数据 同一时刻只有一个线程调用该函数
void Session::SendData(const char* data, size_t len)
{
    if (_socket)
        _socket->async_write_some(boost::asio::buffer(data, len),
            boost::bind(&Session::SendComplete, shared_from_this(), 
                                boost::asio::placeholders::error, 
                                len,
                                boost::asio::placeholders::bytes_transferred));
}

void Session::SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred)
{
    if (err)
    {
        cout << err.message() << endl;
        return;
    }
    assert(bytes_to_transfer == bytes_transferred);

    _send_total += bytes_transferred;

    size_t len;
    const char* data = _send_buf.PopAll(len);
    if (data) // 如果还有数据  继续发送
    {
        SendAsync(data, len);
    }
    else
    {
        _sending_lock.UnLock();
    }
}

// 主动断开连接
void Session::DisConnect(bool notifyServer)
{
    if (_closed_lock.TryLock())
    {
        if (_decoder)
        {
            // 通知业务层 连接已断开 进行相关逻辑业务处理
            if (notifyServer)
            {
                _decoder(nullptr, 0);
            }
            _decoder = nullptr;
        }

        if (_socket && _socket->available())
            _socket->close();
        _socket = nullptr;
    }
}