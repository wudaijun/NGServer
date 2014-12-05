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
        mapservice->SetSessionManager(_session_manager);

        _mapservices.push_back(mapservice);
    }
}

bool LoginService::ProcessMsg(UserMessage* msg)
{
    UserMessageT<PlayerSessionPtr>* msgT = dynamic_cast<UserMessageT<PlayerSessionPtr>*>(msg);
    if (msgT == nullptr)
        return true;

    PlayerSessionPtr session = msgT->GetClient();
    int32_t sid = session->GetSid();

    // ���Ƿ��͸���ǰ�������Ϣ ת��
    if (sid != _sid)
    {
        return ServiceManager::Send(sid, msg);
    }

    // �ͻ��˶Ͽ�����
    if (msg->_len == 0)
    {
        _logging.erase(session->GetConnId());
        _session_manager->RemoveSession(session->GetConnId());
        return true;
    }


    ProtocolReader reader(msg->_data, msg->_len);
    uint16_t msgid = reader.ReadMsgId();
    CallBackType cbType = _calltype[msgid];
    switch (cbType)
    {
    case CallBackType::cbSessioDelegate:
        auto arg = std::pair<PlayerSession*, ProtocolReader&>(session.get(), reader);
        _session_agent.Call(msgid, arg);
        break;
    }

    return true;
}

// �����Ϣע��
bool LoginService::Init()
{
    //RegistPlayer(C2S_Login::msgid, &LoginService::OnLogin, this);
    //RegistPlayer(C2S_Login::msgid, &LoginService::OnLogin, this);
    RegistSession(C2S_Login::msgid, &LoginService::OnLogin, this);

    return true;
}


void LoginService::OnLogin(PlayerSession* player, C2S_Login& msg)
{

}