#include "../../gamebasic/ServiceManager.h"
#include "GameService.h"
#include "../Player/PlayerSession.h"
#include "../Player/Player.h"
#include "SessionManager.h"

bool GameService::ProcessMsg(InsideMessage* msg)
{
    if (!msg)
        return false;

    CallBackType cbType = _calltype[msg->_msgid];
    switch (cbType)
    {
    case cbInsideDelegate:
        _inside_delegate.Call(msg->_msgid, *msg);
        break;

    case cbSessionInsideDelegate:
    {
        PlayerSessionPtr session = _session_manager->GetSession(msg->_sessionid);
        std::pair<PlayerSession*, InsideMessage*> arg = std::make_pair(session.get(), msg);
        _session_inside_delegate.Call(msg->_msgid, arg);
    }
        break;

    case cbPlayerInsideDelegate:
    {
        PlayerPtr player = GetPlayer(msg->_sessionid);
        std::pair<Player*, InsideMessage*> arg = std::make_pair(player.get(), msg);
        _player_inside_delegate.Call(msg->_msgid, arg);
    }
        break;

    default:
        break;
    }
    return true;
}

bool GameService::ProcessMsg(UserMessage* msg)
{
    if (!msg) return false;

    UserMessageT<PlayerSessionPtr>* msgT = dynamic_cast<UserMessageT<PlayerSessionPtr>*>(msg);
    if (msgT == nullptr)
        return false;

    PlayerSessionPtr session = msgT->GetClient();
    if (!session) return false;

    int32_t sid = session->GetSid();
    if (sid != _sid)
    {
        // 不是本服务的消息 转发
        return ServiceManager::Send(sid, msg);
    }

    
    if (msg->_len == 0)
    {
        OnPlayerLogout(session.get());
        return true;
    }
    ProtocolReader reader(msg->_data, msg->_len);
    uint16_t msgid = reader.ReadMsgId();
    CallBackType cbType = _calltype[msgid];
    
    switch (cbType)
    {
        case CallBackType::cbSessioDelegate:
        {
            // 回调
            assert(session != nullptr);
            auto arg = std::pair<PlayerSession*, ProtocolReader&>(session.get(), reader);
            _session_delegate.Call(msgid, arg);
        }
        break;

        case CallBackType::cbPlayerDelegate:
        {
            std::shared_ptr<Player> player = session->GetPlayerToken();
            assert(player != nullptr);
            auto arg = std::pair<Player*, ProtocolReader&>(player.get(), reader);
            _player_delegate.Call(msgid, arg);
        }
    }

    return true;
}

void GameService::OnPlayerLogout(PlayerSession* session)
{
    _session_manager->RemoveSession(session->GetConnId());
}