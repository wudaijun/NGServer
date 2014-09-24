#ifndef __NGSERVER_PLAYERMANAGER_H_INCLUDE__
#define __NGSERVER_PLAYERMANAGER_H_INCLUDE__

#include "../../gamebasic/AsyncTcpListener.h"
#include "Player.h"
#include <map>
/*
*   文件名：PlayerManager.h 定义PlayerManager类
*   PlayerManager类：
*       管理所有的Player 它实际上是一个AsyncTcpListener，主要负责：
*       1. 监听新的客户端连接 并创建Player
*       2. 管理所有在线用户
*       3. 进行玩家下线相关处理
*/

class PlayerManager : public AsyncTcpListener
{
public:
    void OnConnect(const std::shared_ptr<Socket>& socket) override;

    void AddPlayer(const PlayerPtr& player);
private:
    // connid -> Player
    std::map<int32_t, PlayerPtr> _all_players;
};

#endif
