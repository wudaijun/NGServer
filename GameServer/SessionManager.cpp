#include "SessionManager.h"
#include "gamebasic/Session.h"
#include "gamebasic/ServiceManager.h"
#include "Service/LoginService.h"
#include "Player/PlayerSession.h"


void SessionManager::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t id = ++_connect_id;

    // ����PlayerSession
    std::shared_ptr<PlayerSession> session = std::make_shared<PlayerSession>(socket, id);
    std::function<int32_t(const char*, size_t len)> decoder = std::bind(&PlayerSession::Decode, session, std::placeholders::_1, std::placeholders::_2);

    // ע�� decoder������ĳ�Ա��������������������ָ��������á�Ҳ����Session�ڣ�����һ��ָ�����������ָ�롣
    // ������ͷ�PlayerSessionʱ����Ҫ�ֶ��⿪�����໥���á�����PlayerSession�޷���ȷ�ͷš�
    // ��PlayerSession::Close�У�ͨ��_decoder=nullptr;�⿪��������
    session->SetDecoder(decoder); 
    session->SetSid(LoginService::sDefaultSid);

    // ��ӻỰ
    AddSession(session);

    // �����Ự
    session->StartRecv();
}

void SessionManager::AddSession(const PlayerSessionPtr& session)
{
    AutoLocker aLock(&_sessions_lock);
    _sessions[session->GetConnId()] = session;
}

void SessionManager::RemoveSession(int32_t connid)
{
    AutoLocker aLock(&_sessions_lock);
    _sessions.erase(connid);
}

PlayerSessionPtr SessionManager::GetSession(uint32_t connid)
{
    AutoLocker aLock(&_sessions_lock);
    auto item = _sessions.find(connid);
    if (item != _sessions.end())
    {
        return item->second;
        
    }
    else
    {
        return nullptr;
        
    }

}

void SessionManager::Stop()
{
    // �رյײ��������
    AsyncTcpListener::Stop();
    // �ر����лỰ
    AutoLocker aLock(&_sessions_lock);

    for (auto s : _sessions)
    {
        s.second->DisConnect(false);
    }
    _sessions.clear();
}