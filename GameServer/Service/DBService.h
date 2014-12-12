#ifndef __NGSERVER_DBSERVICE_H_INCLUDE__
#define __NGSERVER_DBSERVICE_H_INCLUDE__

#include "GameService.h"
#include "protocol/protocol.h"

class DBService : public Service
{
public:
    DBService(int32_t sid) :
        Service(sid){}

public:
    bool Init() override;

    bool ProcessMsg(InsideMessage* msg) override;

// 响应函数
public:

    void OnPlayerLogin(S2D_PlayerLogin& msg);
    void OnPlayerLoad(S2D_LoadPlayer& msg);

private:
// 注册消息
    template<typename MsgEnum, typename F>
    void Regist(MsgEnum msgid, F f)
    {
        _inside_delegate.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void Regist(MsgEnum msgid, F f, ObjT* obj)
    {
        _inside_delegate.Regist((uint16_t)msgid, f, obj);
    }

// 发送消息
    template <typename MsgEnum, typename MsgT> 
    bool SendMsg(MsgEnum id, MsgT& t)
    {
        if (_from_sid != 0)
            return Service::SendMsg(_from_sid, _from_sessionid, static_cast<uint16_t>(id), t);
        return false;
    }

    template<typename MsgT>
    bool SendMsg(MsgT& t)
    {
        return SendMsg(MsgT::msgid, t);
    }

private:
    DelegateManager<InsideMessage> _inside_delegate;
    int32_t _from_sid = 0;
    int64_t _from_sessionid = 0;
};

typedef std::shared_ptr<DBService> DBServicePtr;
#endif