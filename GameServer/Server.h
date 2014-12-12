#ifndef __NGSERVER_SERVER_H_INCLUDE__
#define __NGSERVER_SERVER_H_INCLUDE__

#include "gamebasic/AsyncTcpListener.h"
#include "gamebasic/Session.h"
#include "SessionManager.h"
#include "gamebasic/ServiceManager.h"
#include "protocol/protocol.h"
#include "Service/LoginService.h"

using namespace NGServer::protocol;
using namespace std;

class Server
{
public:
    Server();

    bool StartServer(const std::string& ip, uint16_t port, int32_t threadNum);

    bool StopServer();

    void IoRun();

    int32_t GetCurOnline();

private:
    boost::asio::io_service _io_service;
    std::shared_ptr<SessionManager> _session_manager;
    std::shared_ptr<LoginService> _login_service;
    std::thread* _io_thread = nullptr;
};

#endif