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
    PlayerManager(IOService* ioservice)
        :AsyncTcpListener(ioservice){}

    void OnConnect(const std::shared_ptr<Socket>& socket) override;

    void AddPlayer(const PlayerPtr& player);
    void RemovePlayer(int32_t connid);

// ------- For Test --------------
    void PrintPlayerName(int32_t connid)
    {
        std::string name = "None";
        if (_all_players.find(connid) != _all_players.end())
        {
            name = _all_players[connid]->_name;
        }
        cout << "Player[" << connid << "] Name: " << name << endl;
    }
//-------------------------------
private:
    // connid -> Player
    std::map<int32_t, PlayerPtr> _all_players;
};

#endif
