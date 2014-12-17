#include "MapService.h"
#include "gamebasic/ServiceManager.h"
#include "Player/Player.h"
#include "SessionManager.h"

bool MapService::Init()
{
    // 消息注册
    RegistInside(kSS_PlayerLogin, &MapService::OnPlayerLogin, this);
    RegistPlayerInside(kD2S_PlayerLoadCompleted, &Player::OnDBLoadCompleted);

    return true;
}

std::shared_ptr<Player> MapService::GetPlayer(int64_t playerid)
{
    auto item = _online_players.find(playerid);
    if (item != _online_players.end())
        return item->second;
    else
        return nullptr;
}

void MapService::AddPlayer(std::shared_ptr<Player>& player)
{
    _online_players[player->GetPlayerId()] = player;
}

void MapService::RemovePlayer(int64_t playerid)
{
    _online_players.erase(playerid);
}

void MapService::OnPlayerLogin(SS_PlayerLogin& msg)
{
    PlayerSessionPtr session = msg.session;
    if (session == nullptr)
        return;

    int64_t playerid = msg.login_info.playerid;
    // 创建 Player 并与PlayerSession关联
    PlayerPtr player = std::make_shared<Player>(playerid, session);
    player->SetMapService(dynamic_pointer_cast<MapService>(shared_from_this()));
    session->SetPlayerToken(player);
    session->SetSid(GetSid()); 
    
    AddPlayer(player);
    
    // 向数据库加载玩家信息
    // ...
    S2D_LoadPlayer loadmsg;
    loadmsg.playerid = playerid;
    SendToDB(playerid, kS2D_LoadPlayer, loadmsg);
}

void MapService::OnPlayerLogout(PlayerSession* session)
{
    RemovePlayer(session->GetPlayerToken()->GetPlayerId());
    _session_manager->RemoveSession(session->GetConnId());
}