
#include "Client.h"

int main()
{
    IOService io;
    TcpClient* cls[1000];
    for (int i = 1; i < 1000; i++)
    {
        cls[i] = new TcpClient(&io, i);
        if (cls[i]->ConnectToServer("127.0.0.1", 23456))
            cls[i]->SendData("Hi. I Am Clinet.! ");
    }

    io.run();
    return 0;
}