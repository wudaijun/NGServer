#include "Client.h"

bool TcpClient::ConnectToServer(const std::string& ip, int32_t port)
{
    return ConnectToServer(EndPoint(boost::asio::ip::address::from_string(ip), port));
}

bool TcpClient::ConnectToServer(EndPoint ep)
{
    boost::system::error_code err;
    _socket.connect(ep, err);
    if (err)
    {
        cout << err.message() << endl;
        return false;
    }

    return true;
}

void TcpClient::SendData(const char* data, size_t len)
{
    if (len == 0) len = strlen(data);
    _socket.async_write_some(boost::asio::buffer(data, len),
        boost::bind(&TcpClient::SendComplete, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::SendComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
        return;

    _socket.async_read_some(boost::asio::buffer(_read_buf, sizeof(_read_buf)), 
        boost::bind(&TcpClient::RecvComplete, this, boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void TcpClient::RecvComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
        return;

    string str(_read_buf, bytes_transferred);
    cout << "Server Say : " << str << endl;
}