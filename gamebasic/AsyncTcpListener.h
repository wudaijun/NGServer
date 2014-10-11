#ifndef __NGSERVER_ASYNCTCPLISTENTER_H_INCLUDE__
#define __NGSERVER_ASYNCTCPLISTENTER_H_INCLUDE__

#include "NetHead.h"

class AsyncTcpListener
{
public:
    AsyncTcpListener(const AsyncTcpListener&) = delete;
    AsyncTcpListener& operator = (const AsyncTcpListener&) = delete;
    

public:
    AsyncTcpListener(IOService* ioservice);

    void Start(const EndPoint& endpoint);
    void Start(const std::string ip, int32_t port)
    {
        Start(EndPoint(boost::asio::ip::tcp::v4(), port));
    }

    virtual void Stop();
    virtual void OnConnect(const std::shared_ptr<Socket>& socket);


private:
    void StartAccept(const std::shared_ptr<Accepter>& acceptor);
    void AcceptCompleted(const boost::system::error_code& err,
        const std::shared_ptr<Accepter>& acceptor,
        const std::shared_ptr<Socket>& socket);

protected:
    atomic_uint32_t _connect_id = 0;

private:
    IOService* _ioservice;
    Locker _lock;
    bool _running;
};

#endif