#include "LoginService.h"

uint16_t LoginService::sDefaultSid = 1;

void LoginService::StartMapServer(boost::asio::io_service* io)
{
    uint16_t sid = _sid;

    // ��־����
    auto logservice = ServiceManager::Spawn<LoginService>(++sid);
    SetLogService(logservice->GetSid());

    // ���ݿ����
    auto dbservice = ServiceManager::Spawn<DBService>(++sid);
    SetDBService(dbservice->GetSid());

    // ����������ͼ����
    for (int i = 0; i < 2; i++)
    {
        auto mapservice = ServiceManager::Spawn<MapService>(++sid);
        mapservice->SetLogService(logservice->GetSid());
        mapservice->SetDBService(dbservice->GetSid());
        mapservice->SetPlayerManager(_player_manager);

        _mapservices.push_back(mapservice);
    }
}

// �����Ϣע��
bool LoginService::Init()
{
    //RegistPlayer(C2S_Login::msgid, &LoginService::OnLogin, this);
    RegistPlayer(C2S_Login::msgid, &LoginService::OnLogin, this);

    return true;
}