#include "PlayerManager.h"
#include "../../gamebasic/Session.h"
#include "../../gamebasic/ServiceManager.h"

void PlayerManager::OnConnect(const std::shared_ptr<Socket>& socket)
{
    uint32_t id = ++_connect_id;

    // ����Player��Session ����Player��Session����
    std::shared_ptr<Session> session = std::make_shared<Session>(socket, id);
    std::shared_ptr<Player> player = std::make_shared<Player>(session, ServiceManager::GetDefaultSid());
    std::function<int32_t(const char*, size_t len)> decoder = std::bind(&Player::Decode, player, std::placeholders::_1, std::placeholders::_2);
    player->SetConnId(id);
    session->SetDecoder(decoder);

    AddPlayer(player);
}

void PlayerManager::AddPlayer(const PlayerPtr& player)
{
    _all_players[player->GetConnId()] = player;
}