#include "SessionManager.h"
#include "gamebasic/Session.h"
#include "gamebasic/ServiceManager.h"
#include "Service/LoginService.h"
#include "Player/PlayerSession.h"


void SessionManager::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t id = ++_connect_id;

    // 创建PlayerSession
    std::shared_ptr<PlayerSession> session = std::make_shared<PlayerSession>(socket, id);
    std::function<int32_t(const char*, size_t len)> decoder = std::bind(&PlayerSession::Decode, session, std::placeholders::_1, std::placeholders::_2);

    // 注意 decoder是自身的成员函数，因此这里存在智能指针的自引用。也就是Session内，包含一个指向自身的智能指针。
    // 因此在释放PlayerSession时，需要手动解开这种相互引用。否则PlayerSession无法正确释放。
    // 在PlayerSession::Close中，通过_decoder=nullptr;解开这种引用
    session->SetDecoder(decoder); 
    session->SetSid(LoginService::sDefaultSid);

    // 添加会话
    AddSession(session);

    // 启动会话
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
    // 关闭底层网络监听
    AsyncTcpListener::Stop();
    // 关闭所有会话
    AutoLocker aLock(&_sessions_lock);

    for (auto s : _sessions)
    {
        s.second->DisConnect(false);
    }
    _sessions.clear();
}