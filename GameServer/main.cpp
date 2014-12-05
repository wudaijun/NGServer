#include "gamebasic/AsyncTcpListener.h"
#include "gamebasic/Session.h"
#include "Player/SessionManager.h"
#include "gamebasic/ServiceManager.h"
#include "protocol/protocol.h"
#include "Service/LoginService.h"

using namespace NGServer::protocol;

int main()
{
    boost::asio::io_service ioservice;
    SessionManager listener(&ioservice);
    //ServiceManager::PushCycleMessage(CycleMessage::Create(nullptr, 10000*1000));
    //ServiceManager::PushTimerMessage(new ServiceTimer(Service::SteadyNow() + 10000 * 2000, 0, new TimerMessage("2s")));

    auto login = ServiceManager::Spawn<LoginService>(LoginService::sDefaultSid);
    login->SetSessionManager(&listener);
    login->StartMapServer(&ioservice);

    listener.Start("127.0.0.1", 23456);

    ServiceManager::Start(2);
    ioservice.run();

    return 0;
}