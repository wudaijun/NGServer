#include "../../gamebasic/ServiceManager.h"
#include "GameService.h"

bool GameService::ProcessMsg(UserMessage* msg)
{
    UserMessageT<PlayerPtr>* msgT = dynamic_cast<UserMessageT<PlayerPtr>*>(msg);
    if (msgT == nullptr)
        return true;

    PlayerPtr player = msgT->GetClient();
    int32_t sid = player->GetSid();

    // 不是发送给当前服务的消息 转发
    if (sid != _sid)
    {
        return ServiceManager::Send(sid, msg);
    }

    // 客户端断开连接
    if (msg->_len == 0)
    {
        player->Offline();
        _player_manager->RemovePlayer(player->GetConnId());
        return true;
    }


    ProtocolReader reader(msg->_data, msg->_len);
    uint16_t msgid = reader.ReadMsgId();
    CallBackType cbType = _calltype[msgid];
    switch (cbType)
    {
    case cbPlayerAgent:
        auto arg = std::pair<Player&, ProtocolReader&>(*player, reader);
        _player_agent.Call(msgid, arg);
        break;
    }

    return true;
}

bool GameService::ProcessMsg(InsideMessage* msg)
{
    return true;
}