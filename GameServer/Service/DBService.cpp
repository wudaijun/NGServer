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
    // 加载用户数据
    // .....
    
    // 这里简单使用自增ID作为玩家ID
    // 而实际上应该从数据库中加载(登录时)或生成(注册时)

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
    // 加载玩家业务数据
    // ...

    D2S_PlayerLoadCompleted res;
    res.playerid = msg.playerid;
    SendMsg(res);
}