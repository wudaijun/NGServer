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

    bool IsAllOffline();

    int32_t GetCurOnline();

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

    static uint16_t sDefaultSid;


#pragma region 回调函数
public:
    void OnPlayerLogin(PlayerSession* session, C2S_Login& msg);
    void OnDBPlayerLogin(PlayerSession* session, D2S_PlayerLoginResult& msg);
    void OnPlayerLogout(PlayerSession* session) override;
#pragma endregion

private:
    // 该LoginService管理的所有MapService
    std::vector<MapServicePtr> _mapservices;
    // 正在登录中的用户 key : 用户名  value : 用户连接ID
    std::unordered_map<std::string, uint32_t> _logging;
};

#endif