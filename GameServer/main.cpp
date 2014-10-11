#include "../gamebasic/AsyncTcpListener.h"
#include "../gamebasic/Session.h"
#include "Player/PlayerManager.h"
#include "../gamebasic/ServiceManager.h"
int main()
{
    boost::asio::io_service ioservice;
    PlayerManager listener(&ioservice);

    ServiceManager::Start(1);
    //ServiceManager::PushCycleMessage(CycleMessage::Create(nullptr, 10000*1000));
    //ServiceManager::PushTimerMessage(new ServiceTimer(Service::SteadyNow() + 10000 * 2000, 0, new TimerMessage("2s")));

    listener.Start("127.0.0.1", 23456);
    ioservice.run();

    return 0;
}