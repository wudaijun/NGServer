#include "Client.h"

bool TcpClient::ConnectToServer(const std::string& ip, int32_t port)
{
    return ConnectToServer(EndPoint(boost::asio::ip::tcp::v4(), port));
}

bool TcpClient::ConnectToServer(EndPoint ep)
{
    _socket.connect(ep);

    return true;
}

bool TcpClient::SendData(const char* data, size_t len)
{
    _socket.async_write_some(boost::asio::buffer(data, len),
        boost::bind(&TcpClient::SendComplete, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::SendComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
        return;

    _socket.async_read_some(boost::asio::buffer(_read_buf, 65535), 
        boost::bind(&TcpClient::RecvComplete, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::RecvComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
        return;

    cout << "Server Say : "
}