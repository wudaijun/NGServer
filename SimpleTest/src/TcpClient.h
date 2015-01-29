#ifndef __SIMPLE_TEST_TCPCLIENT_H_INCLUDE__
#define __SIMPLE_TEST_TCPCLIENT_H_INCLUDE__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include<iostream>
#include<vector>
#include<chrono>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../protocol/protocol.h"
#include "../../common/AutoCall.h"

using namespace std;
using namespace boost;
typedef boost::asio::io_service IOService;
typedef boost::asio::ip::tcp::endpoint EndPoint;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::socket_base::shutdown_type ShutDownType;
using namespace NGServer::protocol;
using namespace NGServer::common;

class TcpClient : public std::enable_shared_from_this<TcpClient>
{
#pragma region ÍøÂçIO

public:
    TcpClient(IOService* ioservice, int32_t id = 0);

    ~TcpClient();

    virtual bool Init();

    inline void SetServAddress(std::string serv_address, short serv_port)
    {
        _serv_address = serv_address;
        _serv_port = serv_port;
    }

    bool ConnectToServer();

    bool ConnectToServer(EndPoint ep);

    virtual void Connected(const boost::system::error_code& err);

    void StartRecv();

    void SendData(const char* data, size_t len=0);

    template<typename MsgT>
    void SendMsg(MsgT msg)
    {
        const static int kBuffSize = 256 * 256;
        char buff[kBuffSize];
        ProtocolWriter writer(buff, kBuffSize);
        
        AutoEncode(writer, msg);
        size_t len = writer.WriteHead(MsgT::msgid);
        SendData(buff, len);
    }

    void SendComplete(const boost::system::error_code& err,  size_t bytes_transferred);

    void RecvComplete(const boost::system::error_code& err, size_t bytes_transferred);

    int32_t Decode(const char* data, size_t len);

    void Close(ShutDownType stype = ShutDownType::shutdown_both);
#pragma endregion

#pragma region ÏûÏ¢×¢²á
public:
    template<typename MsgEnum, typename F>
    void RegistMsg(MsgEnum id, F f)
    {
        _delegate.Regist(static_cast<uint16_t>(id), f);
    }

    template < typename MsgEnum, typename F, typename ObjT > 
    void RegistMsg(MsgEnum id, F f, ObjT* obj)
    {
        _delegate.Regist(static_cast<uint16_t>(id), f, obj);
    }
#pragma endregion

protected:
    static const size_t kBuffSize = 65535;
    IOService* _ioservice;
    Socket _socket;
    int32_t _id;
    char _read_buf[kBuffSize];
    size_t _offset = 0;
    DelegateManager <ProtocolReader> _delegate;
    std::string _serv_address;
    short _serv_port;
};

#endif
