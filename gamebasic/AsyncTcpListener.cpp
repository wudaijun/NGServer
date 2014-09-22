#include "AsyncTcpListener.h"
#include "Session.h"

AsyncTcpListener::AsyncTcpListener(IOService* ioservice)
    :_ioservice(ioservice)
{
}

void AsyncTcpListener::Start(const EndPoint& endpoint)
{
    {
        AutoLocker locker(&_lock);
        if (_running) return;
        _running = true;
    }

    std::shared_ptr<Accepter> acceptor = std::make_shared<Accepter>(*_ioservice, endpoint, false);
    acceptor->set_option(Accepter::reuse_address(true));
    acceptor->set_option(boost::asio::socket_base::keep_alive(true));
    StartAccept(acceptor);
}

void AsyncTcpListener::StartAccept(const std::shared_ptr<Accepter>& acceptor)
{
    if (_running)
    {
        std::shared_ptr<Socket> socket(new Socket(*_ioservice));
        acceptor->async_accept(*socket, boost::bind(&AsyncTcpListener::AcceptCompleted, this, boost::asio::placeholders::error, acceptor, socket));
    }
}

void AsyncTcpListener::AcceptCompleted(const boost::system::error_code& err,
    const std::shared_ptr<Accepter>& acceptor,
    const std::shared_ptr<Socket>& socket)
{
    if (err)
        return;

    if (_running)
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
        StartAccept(acceptor);
    }
}

void AsyncTcpListener::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t conn_id = ++_connect_id;
    std::shared_ptr<Session> session = std::make_shared<Session>(socket, conn_id);
#ifdef _DEBUG
    cout << " -- CONNECTED : [" << conn_id << "]" << endl;
#endif
    if (session) session->StartRecv();
}

// 停止监听
void AsyncTcpListener::Stop()
{
    AutoLocker(&_lock);
    if (_running) _running = false;
}