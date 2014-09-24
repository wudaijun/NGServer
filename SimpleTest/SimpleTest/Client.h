#include <boost\asio.hpp>
#include <boost\bind.hpp>
#include<iostream>

using namespace std;
using namespace boost;
typedef boost::asio::io_service IOService;
typedef boost::asio::ip::tcp::endpoint EndPoint;
typedef boost::asio::ip::tcp::socket Socket;

class TcpClient
{
public:
    TcpClient(IOService* ioservice, int32_t id=0) :
        _ioservice(ioservice), _socket(*ioservice), _id(id){}

    bool ConnectToServer(const std::string& ip, int32_t port);

    bool ConnectToServer(EndPoint ep);

    void SendData(const char* data, size_t len=0);

    void SendComplete(const boost::system::error_code& err,  size_t bytes_transferred);

    void RecvComplete(const boost::system::error_code& err, size_t bytes_transferred);
private:
    IOService* _ioservice;
    Socket _socket;
    int32_t _id;
    char _read_buf[65535];
};