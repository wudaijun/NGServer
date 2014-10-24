#include "PlayerManager.h"
#include "../../gamebasic/Session.h"
#include "../../gamebasic/ServiceManager.h"
#include "../Service/LoginService.h"

void PlayerManager::OnConnect(const std::shared_ptr<Socket>& socket)
{
#ifdef _DEBUG
    cout << " ^^ Player[" << _connect_id+1 << "] Login." << endl;
#endif
    uint32_t id = ++_connect_id;

    // 创建Player和Session 并将Player和Session关联
    std::shared_ptr<Session> session = std::make_shared<Session>(socket, id);
    std::shared_ptr<Player> player = std::make_shared<Player>(session, LoginService::sDefaultSid);
    std::function<int32_t(const char*, size_t len)> decoder = std::bind(&Player::Decode, player, std::placeholders::_1, std::placeholders::_2);
    player->SetConnId(id);
    session->SetDecoder(decoder);

    AddPlayer(player);

    session->StartRecv();
}

void PlayerManager::AddPlayer(const PlayerPtr& player)
{
    _all_players[player->GetConnId()] = player;
}

void PlayerManager::RemovePlayer(int32_t connid)
{
    _all_players.erase(connid);
}