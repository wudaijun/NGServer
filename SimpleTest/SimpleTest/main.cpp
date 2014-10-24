
#include "Client.h"
#include "../../protocol/protocol.h"

using namespace NGServer::protocol;
int main()
{
    IOService io;
    /*
    TcpClient* cls[1000];
    for (int i = 1; i < 1000; i++)
    {
        cls[i] = new TcpClient(&io, i);
        if (cls[i]->ConnectToServer("127.0.0.1", 23456))
            cls[i]->SendData("Hi. I Am Clinet.! ");
    }
    */
    
    C2S_Login msg;
    msg.name = "wudaijun";
    msg.pwd = "123";

    TcpClient cls(&io, 1);
    cls.ConnectToServer("127.0.0.1", 23456);
    cls.SendMsg(msg);

    io.run();
    return 0;
}