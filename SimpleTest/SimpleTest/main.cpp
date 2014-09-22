
#include "Client.h"

int main()
{
    IOService io;
    TcpClient client(&io);
    if(client.ConnectToServer("127.0.0.1", 23456))
        client.SendData("Hi. I Am Clinet.! ");
    io.run();
    return 0;
}