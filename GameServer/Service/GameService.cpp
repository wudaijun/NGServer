#include "../../gamenet/ServiceManager.h"
#include "GameService.h"

bool GameService::ProcessMsg(UserMessage* msg)
{
    UserMessageT<PlayerPtr>* msgT = dynamic_cast<UserMessageT<PlayerPtr>*>(msg);
    if (msgT == nullptr)
        return true;

    PlayerPtr player = msgT->GetClient();
    int32_t sid = player->GetSid();

    // ���Ƿ��͸���ǰ�������Ϣ ת��
    if (sid != _sid)
    {
        return ServiceManager::Send(sid, msg);
    }

    // �ͻ��˶Ͽ�����
    if (msg->_len == 0)
    {
        player->Offline();
        _player_manager.RemovePlayer(player->GetConnId());
        return true;
    }


    ProtocolReader reader(msg->_data, msg->_len);
    uint16_t msgid = reader.ReadMsgId();
    CallBackType cbType = _calltype[msgid];
    switch (cbType)
    {
    case cbPlayerAgent:
        _player_agent.Call(msgid, *player, reader);
        break;
    }

    return true;
}