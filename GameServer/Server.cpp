#include "Server.h"

Server::Server()
{
}

void Server::IoRun()
{
    _io_service.run();
}

bool Server::StartServer(const std::string& ip, uint16_t port, int32_t threadNum)
{
    //ServiceManager::PushCycleMessage(CycleMessage::Create(nullptr, 10000*1000));
    //ServiceManager::PushTimerMessage(new ServiceTimer(Service::SteadyNow() + 10000 * 2000, 0, new TimerMessage("2s")));

    _session_manager = std::make_shared<SessionManager>(&_io_service);
    _login_service = ServiceManager::Spawn<LoginService>(LoginService::sDefaultSid);
    _login_service->SetSessionManager(_session_manager);
    _login_service->StartMapServer(&_io_service);

    // 开启消息服务
    ServiceManager::Start(threadNum);

    // 开启网络服务
    _session_manager->Start(ip, port);

    _io_thread = new std::thread(&Server::IoRun, this);
    return true;
}

bool Server::StopServer()
{
    // 关闭网络
    _session_manager->Stop();
    // 检查是否所有玩家都已下线 
    if (!_login_service->IsAllOffline())
        sleep(1000); //等待一秒

    // 停止消息服务
    ServiceManager::Stop();
    // 停止网络
    _io_service.stop();

    if (_io_thread) _io_thread->join();

    return true;
}

int32_t Server::GetCurOnline()
{
    return _login_service->GetCurOnline();
}
