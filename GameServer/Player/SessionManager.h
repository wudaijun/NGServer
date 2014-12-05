#ifndef __NGSERVER_SessionManager_H_INCLUDE__
#define __NGSERVER_SessionManager_H_INCLUDE__

#include "gamebasic/AsyncTcpListener.h"
#include "PlayerSession.h"
#include <map>
/*
*   文件名：SessionManager.h 定义SessionManager类
*   SessionManager类：
*       管理所有的会话PlayerSession 它实际上是一个AsyncTcpListener，主要负责：
*       1. 监听新的客户端连接 并创建PlayerSession
*       2. 管理所有活动会话
*       3. 进行玩家下线相关处理
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
