#include "Session.h"

void Session::StartRecv()
{
    if (_run)
        return;

    _run = true;
    AsyncReadSome();
}

void Session::AsyncReadSome()
{
    _socket->async_read_some(
        boost::asio::buffer(_recv_buf + _recv_off, kBufferSize - _recv_off),
        boost::bind(&Session::ReadComplete, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void Session::ReadComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
        return;

    _recv_total += bytes_transferred;
    _recv_off += bytes_transferred;

    // 处理数据
    ProcessData();
    // 继续接收数据
    AsyncReadSome();
}

void Session::ProcessData()
{
#ifdef _DEBUG
    string str(_recv_buf, _recv_off);
    cout << " -- RECV FROM [" << _id << "]: " << str << endl;
    SendAsync("Hello. this is Server.", strlen("Hello. this is Server."));
#endif


    int32_t remain = 0;
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
}

// 发送数据
bool Session::SendAsync(const char* data, size_t len)
{
    if (!_run)
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
        assert(0); // 发送缓冲区满
}

void Session::SendData(const char* data, size_t len)
{
    _socket->async_write_some(boost::asio::buffer(data, len),
        boost::bind(&Session::SendComplete, shared_from_this(), 
                                boost::asio::placeholders::error, 
                                len,
                                boost::asio::placeholders::bytes_transferred));
}

void Session::SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred)
{
    if (err)
        return;

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