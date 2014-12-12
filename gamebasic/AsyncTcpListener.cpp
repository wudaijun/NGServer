#include "AsyncTcpListener.h"
#include "Session.h"

AsyncTcpListener::AsyncTcpListener(IOService* ioservice)
    :_ioservice(ioservice)
{
}

void AsyncTcpListener::Start(const EndPoint& endpoint)
{
    {
        AutoLocker aLock(&_acceptor_lock);
        if (_acceptor != nullptr)
            return;

        _acceptor = std::make_shared<Accepter>(*_ioservice, endpoint, false);
    }

    _acceptor->set_option(Accepter::reuse_address(true));
    _acceptor->set_option(boost::asio::socket_base::keep_alive(true));
    
    StartAccept();
}

void AsyncTcpListener::StartAccept()
{
    if (_acceptor != nullptr)
    {
        std::shared_ptr<Socket> socket(new Socket(*_ioservice));
        _acceptor->async_accept(*socket, boost::bind(&AsyncTcpListener::AcceptCompleted, this, boost::asio::placeholders::error, socket));
    }
}

void AsyncTcpListener::AcceptCompleted(const boost::system::error_code& err,
    const std::shared_ptr<Socket>& socket)
{
    if (err)
    {
        return;
    }

    if (_acceptor != nullptr)
    {
        // 对接收到的套接字设置基本选项
        const static boost::asio::ip::tcp::no_delay nodelay(true);
        const static boost::asio::socket_base::send_buffer_size send_buf_size(256 * 256 * 10); // send buffer 640 kb
        const static boost::asio::socket_base::receive_buffer_size recv_buf_size(256 * 256); // recv buffer 64 kb
        socket->set_option(nodelay);
        socket->set_option(send_buf_size);
        socket->set_option(recv_buf_size);

        // 处理 并继续监听
        OnConnect(socket);
        StartAccept();
    }
}

void AsyncTcpListener::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t conn_id = ++_connect_id;
    std::shared_ptr<Session> session = std::make_shared<Session>(socket, conn_id);

    if (session) session->StartRecv();
}

// 停止监听
void AsyncTcpListener::Stop()
{
    if (_closed_lock.TryLock())
    {
        AutoLocker aLock(&_acceptor_lock);
        _acceptor->close();
        _acceptor = nullptr;
    }
}