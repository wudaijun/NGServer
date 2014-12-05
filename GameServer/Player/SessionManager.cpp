#include "SessionManager.h"
#include "gamebasic/Session.h"
#include "gamebasic/ServiceManager.h"
#include "../Service/LoginService.h"
#include "PlayerSession.h"

void SessionManager::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t id = ++_connect_id;

    // ´´½¨PlayerSession
    std::shared_ptr<PlayerSession> session = std::make_shared<PlayerSession>(socket, id);
    std::function<int32_t(const char*, size_t len)> decoder = std::bind(&PlayerSession::Decode, session, std::placeholders::_1, std::placeholders::_2);
    session->SetDecoder(decoder);
    session->SetSid(LoginService::sDefaultSid);

    AddSession(session);

    session->StartRecv();
}

void SessionManager::AddSession(const PlayerSessionPtr& session)
{
    _sessions[session->GetConnId()] = session;
}

void SessionManager::RemoveSession(int32_t connid)
{
    _sessions.erase(connid);
}