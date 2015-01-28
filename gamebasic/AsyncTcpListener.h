#ifndef __NGSERVER_ASYNCTCPLISTENTER_H_INCLUDE__
#define __NGSERVER_ASYNCTCPLISTENTER_H_INCLUDE__

#include "NetHead.h"

class AsyncTcpListener
{
public:
    AsyncTcpListener(const AsyncTcpListener&) = delete;
    AsyncTcpListener& operator = (const AsyncTcpListener&) = delete;
    virtual ~AsyncTcpListener(){};

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
    void StartAccept();
    void AcceptCompleted(const boost::system::error_code& err,
        const std::shared_ptr<Socket>& socket);

protected:
    atomic_uint _connect_id;

private:
    IOService* _ioservice;
    std::shared_ptr<Accepter> _acceptor;
    Locker _acceptor_lock;
    Locker _closed_lock;
};

#endif
