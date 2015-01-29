#include "TcpClient.h"
#include <iostream>

TcpClient::TcpClient(IOService* ioservice, int32_t id /* = 0 */) 
    :_ioservice(ioservice), _socket(*ioservice), _id(id)
{
    //Init();
    _offset = 0;
}

bool TcpClient::Init()
{
    return true;
}

bool TcpClient::ConnectToServer()
{
    return ConnectToServer(EndPoint(boost::asio::ip::address::from_string(_serv_address), _serv_port));
}

bool TcpClient::ConnectToServer(EndPoint ep)
{
    _socket.async_connect(ep, boost::bind(&TcpClient::Connected, shared_from_this(), boost::asio::placeholders::error));
    return true;
}

// 连接回调
void TcpClient::Connected(const boost::system::error_code& err)
{
    if (err)
    {
        std::cerr << "connect error: "<<err.message() << std::endl;
        return;
    }

    StartRecv();
}

void TcpClient::StartRecv()
{
    _socket.async_read_some(boost::asio::buffer(_read_buf+_offset, kBuffSize-_offset),
        boost::bind(&TcpClient::RecvComplete, shared_from_this(), boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::SendData(const char* data, size_t len)
{
    if (len == 0) len = strlen(data);
    _socket.async_write_some(boost::asio::buffer(data, len),
        boost::bind(&TcpClient::SendComplete, shared_from_this(), boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::SendComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
    {
        std::cerr << "send error: " << err.message() << std::endl;
        return;
    }
}

void TcpClient::RecvComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
    {
        std::cerr << "recv error: " << err.message() << std::endl;
        return;
    }
    _offset += bytes_transferred;
    int32_t remain = Decode(_read_buf, _offset);
    if (remain > 0 && remain < kBuffSize)
    {
        size_t remainoff = _offset - remain;
        memcpy(_read_buf, _read_buf + remainoff, remain);
        _offset = remain;
    }
    else
    {
        _offset = 0;
    }

    StartRecv();
}

// 对数据解码 返回解码后缓冲区剩余的长度
int32_t TcpClient::Decode(const char* data, size_t len)
{
    // 服务器主动断开连接
    if (data == nullptr || len == 0)
    {
        return 0;
    }

    const char* buff = data;
    size_t remainLen = len;
    static const uint16_t headLen = ProtocolStream::kHeadLen + ProtocolStream::kMsgIdLen;
    while (remainLen > headLen)
    {
        int32_t msgLen = std::max(headLen, *((uint16_t*)buff));
        if (remainLen < msgLen)
        {
            break;
        }

        remainLen -= msgLen;

        // 将消息解码
        ProtocolReader reader((char*)buff, msgLen);
        uint16_t msgid = reader.ReadMsgId();
        _delegate.Call(msgid, reader);

        buff += msgLen;
    }
    return remainLen;
}

// 主动关闭和服务器的连接
void TcpClient::Close(ShutDownType stype)
{
    if (_socket.available())
    {
        _socket.shutdown(stype);
        _socket.close();
    }
}

TcpClient::~TcpClient()
{
    Close();
}
