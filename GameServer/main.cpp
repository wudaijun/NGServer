#include <iostream>
#include "Server.h"

int main()
{
    
    Server server;

    server.StartServer("127.0.0.1", 23456, 2);

    std::cout << "----- Server Running" << std::endl;
    std::cout << "----- Esc To Exiting Safely. " << std::endl;
    
    while (getchar() != 27);

    std::cout << "----- Cur Online: " << server.GetCurOnline() << std::endl;
    std::cout << "----- Now. Stoping ... ";
    server.StopServer();
    std::cout << " Done. " << endl;

    getchar();

    return 0;
}
