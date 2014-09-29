#ifndef __NGSERVER_PLAYERMANAGER_H_INCLUDE__
#define __NGSERVER_PLAYERMANAGER_H_INCLUDE__

#include "../../gamebasic/AsyncTcpListener.h"
#include "Player.h"
#include <map>
/*
*   �ļ�����PlayerManager.h ����PlayerManager��
*   PlayerManager�ࣺ
*       �������е�Player ��ʵ������һ��AsyncTcpListener����Ҫ����
*       1. �����µĿͻ������� ������Player
*       2. �������������û�
*       3. �������������ش���
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