#ifndef __NGSERVER_LOGINSERVICE_H_INCLUDE__
#define __NGSERVER_LOGINSERVICE_H_INCLUDE__

#include "MapService.h"
#include "LogService.h"
#include "DBService.h"
#include <vector>
#include <unordered_map>
#include "GameService.h"
#include "gamebasic/ServiceManager.h"
/*
*   登录服务 LoginService
*   管理所有玩家的登录和初始化相关
*/
class LoginService : public GameService
{
public:
    LoginService(int32_t sid) :
        GameService(sid){}

    // 完成消息注册
    bool Init() override;

    void StartMapServer(boost::asio::io_service* io);

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

    static uint16_t sDefaultSid;

#pragma region 注册回调
    template<typename MsgEnum, typename F>
    void RegistSession(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_agent.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistSession(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_agent.Regist((uint16_t)msgid, f, obj);
    }
#pragma endregion

#pragma region 处理消息

    bool ProcessMsg(UserMessage* msg) override;

#pragma endregion

#pragma region 回调函数
public:
    void OnLogin(PlayerSession* player, C2S_Login& msg);
#pragma endregion

private:
    // 该LoginService管理的所有MapService
    std::vector<MapServicePtr> _mapservices;
    // 正在登录中的用户
    std::unordered_map<int32_t, PlayerSessionPtr> _logging;

    DelegateManager<std::pair<PlayerSession*, ProtocolReader&>> _session_agent;
};

#endif