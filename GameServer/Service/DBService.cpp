#include "DBService.h"
#include <atomic>

bool DBService::Init()
{
    Regist(DBMsgId::kS2D_PlayerLogin, &DBService::OnPlayerLogin, this);
    Regist(DBMsgId::kS2D_LoadPlayer, &DBService::OnPlayerLoad, this);

    return true;
}

bool DBService::ProcessMsg(InsideMessage* msg)
{
    _from_sid = msg->_srcsid;
    _from_sessionid = msg->_sessionid;
    _inside_delegate.Call(msg->_msgid, *msg);
    return true;
}

void DBService::OnPlayerLogin(S2D_PlayerLogin& msg)
{
    // �����û�����
    // .....
    
    // �����ʹ������ID��Ϊ���ID
    // ��ʵ����Ӧ�ô����ݿ��м���(��¼ʱ)������(ע��ʱ)

    static std::atomic_uint64_t sId = 10000;
    D2S_PlayerLoginResult res;
    res.connid = msg.connid;
    res.owner = msg.owner;
    res.pwd = msg.pwd;
    res.playerid = ++sId;
    SendMsg(res);
}

void DBService::OnPlayerLoad(S2D_LoadPlayer& msg)
{
    // �������ҵ������
    // ...

    D2S_PlayerLoadCompleted res;
    res.playerid = msg.playerid;
    SendMsg(res);
}