#ifndef __NGSERVER_LOGINSERVICE_H_INCLUDE__
#define __NGSERVER_LOGINSERVICE_H_INCLUDE__

#include "MapService.h"
#include "LogService.h"
#include "DBService.h"
#include <vector>
#include "GameService.h"
#include "../../gamebasic/ServiceManager.h"
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

#pragma region 回调函数
public:
    void OnLogin(Player& player, C2S_Login& msg)
    {
        cout << "-- C2S_Login --" << endl;
        cout << "- name: " << msg.name<<endl;
        cout << "- pwd: " << msg.pwd << endl;
        cout << "---------------" << endl;
        player._name = msg.name;
        _player_manager->PrintPlayerName(player.GetConnId());
    }
#pragma endregion

private:
    std::vector<MapServicePtr> _mapservices;
    
};

#endif