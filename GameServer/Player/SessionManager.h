#ifndef __NGSERVER_SessionManager_H_INCLUDE__
#define __NGSERVER_SessionManager_H_INCLUDE__

#include "gamebasic/AsyncTcpListener.h"
#include "PlayerSession.h"
#include <map>
/*
*   �ļ�����SessionManager.h ����SessionManager��
*   SessionManager�ࣺ
*       �������еĻỰPlayerSession ��ʵ������һ��AsyncTcpListener����Ҫ����
*       1. �����µĿͻ������� ������PlayerSession
*       2. �������л�Ự
*       3. �������������ش���
*/

class SessionManager : public AsyncTcpListener
{
public:
    SessionManager(IOService* ioservice)
        :AsyncTcpListener(ioservice){}

    void OnConnect(const std::shared_ptr<Socket>& socket) override;

    void AddSession(const PlayerSessionPtr& session);
    void RemoveSession(int32_t connid);

// ------- For Test --------------
    void PrintPlayerName(int32_t connid)
    {
    }
//-------------------------------
private:
    // connid -> Player
    std::map<int32_t, PlayerSessionPtr> _sessions;
};

#endif
