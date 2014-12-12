#ifndef __NGSERVER_MAPSERVICE_H_INLCUDE__
#define __NGSERVER_MAPSERVICE_H_INLCUDE__

#include "GameService.h"
#include <unordered_map>
#include "../Player/PlayerSession.h"

class MapService : public GameService
{
public:
    MapService(int32_t sid) :
        GameService(sid){}

    // 消息注册和初始化
    bool Init();

    // 得到当前map在线人数
    inline size_t GetOnlineNum() const
    {
        return _online_players.size();
    }

    std::shared_ptr<Player> GetPlayer(int64_t playerid) override;

    void AddPlayer(std::shared_ptr<Player>& player);
    void RemovePlayer(int64_t playerid);

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

public:
    void OnPlayerLogin(SS_PlayerLogin& msg);
    void OnPlayerLogout(PlayerSession* session) override;

private:

    // 当前在线用户 key: 玩家ID value: 玩家指针
    std::unordered_map<int64_t, std::shared_ptr<Player>> _online_players;
};

typedef std::shared_ptr<MapService> MapServicePtr;
#endif