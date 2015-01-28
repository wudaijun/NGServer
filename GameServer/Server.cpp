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

    // ������Ϣ����
    ServiceManager::Start(threadNum);

    // �����������
    _session_manager->Start(ip, port);

    _io_thread = new std::thread(&Server::IoRun, this);
    return true;
}

bool Server::StopServer()
{
    // �ر�����
    _session_manager->Stop();
    // ����Ƿ�������Ҷ������� 
    if (!_login_service->IsAllOffline())
        sleep(1000); //�ȴ�һ��

    // ֹͣ��Ϣ����
    ServiceManager::Stop();
    // ֹͣ����
    _io_service.stop();

    if (_io_thread) _io_thread->join();

    return true;
}

int32_t Server::GetCurOnline()
{
    return _login_service->GetCurOnline();
}
