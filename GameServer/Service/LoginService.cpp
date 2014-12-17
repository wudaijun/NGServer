#include "LoginService.h"
#include "SessionManager.h"

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
        mapservice->SetSessionManager(_session_manager);

        _mapservices.push_back(mapservice);
    }
}

// ����Ƿ�������Ҷ�������
bool LoginService::IsAllOffline()
{
    for (auto map : _mapservices)
    {
        if (map->GetOnlineNum() != 0)
            return false;
    }
    return true;
}

// �õ���ǰ���������
int32_t LoginService::GetCurOnline()
{
    int32_t total_online = 0;
    for (auto map : _mapservices)
    {
        total_online += map->GetOnlineNum();
    }
    return total_online;
}

// �����Ϣע��
bool LoginService::Init()
{
    RegistSession(C2S_Login::msgid, &LoginService::OnPlayerLogin, this);
    RegistSessionInside(D2S_PlayerLoginResult::msgid, &LoginService::OnDBPlayerLogin, this);
    return true;
}

void LoginService::OnPlayerLogin(PlayerSession* session, C2S_Login& msg)
{
    if (!session->IsState(kSessionState_None))
        return;

    session->SetState(kSessionState_Logging);
    session->SetOwner(msg.owner);

    if (_logging.find(msg.owner) != _logging.end())
        return;

    _logging[msg.owner] = session->GetConnId();

    S2D_PlayerLogin dbmsg;
    dbmsg.owner = msg.owner;
    dbmsg.pwd = msg.pwd;
    dbmsg.connid = session->GetConnId();
    SendToDB(dbmsg.connid, kS2D_PlayerLogin, dbmsg);
}

void LoginService::OnDBPlayerLogin(PlayerSession* session, D2S_PlayerLoginResult& msg)
{
    // ����Ƿ��¼�ɹ�
    // ...

    session->SetState(kSessionState_Success);
    _logging.erase(msg.owner);
    // ת����MapService
    // ����ֱ�ӷ�����һ��MapService �������ؾ���
    SS_PlayerLogin res;
    res.login_info = msg;
    res.session = dynamic_pointer_cast<PlayerSession>(session->shared_from_this());
    
    if (_mapservices.size() > 0)
    {
        int32_t sid = _mapservices[0]->GetSid();
        SendMsg(sid, session->GetConnId(), kSS_PlayerLogin, res);
    }

    // ������ҵ�¼���
    S2C_Login loginresult;
    loginresult.login_success = true;
    session->SendMsg(kS2C_Login, loginresult);
}

// �������
void LoginService::OnPlayerLogout(PlayerSession* session)
{
    _session_manager->RemoveSession(session->GetConnId());
    session->DisConnect(false);
}