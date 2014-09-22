#include "../gamebasic/AsyncTcpListener.h"
#include "../gamebasic/Session.h"

int main()
{
    boost::asio::io_service ioservice;
    AsyncTcpListener listener(&ioservice);
    listener.Start("127.0.0.1", 23456);
    ioservice.run();
    return 0;
}