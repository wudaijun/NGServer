#ifndef __NGSERVER_SessionManager_H_INCLUDE__
#define __NGSERVER_SessionManager_H_INCLUDE__

#include "gamebasic/AsyncTcpListener.h"
#include <map>
#include "common/Locker.h"

/*
*   文件名：SessionManager.h 定义SessionManager类
*   SessionManager类：
*       管理所有的会话PlayerSession 它实际上是一个AsyncTcpListener，主要负责：
*       1. 监听新的客户端连接 并创建PlayerSession 并启动会话
*       2. 管理所有活动会话
*
*   由于SessionManager管理所有Service上的玩家会话，为各个Service共有组件
*   因此它必须线程安全
*/

class PlayerSession;

class SessionManager : public AsyncTcpListener
{
public:
    SessionManager(IOService* ioservice)
        :AsyncTcpListener(ioservice){}

    // 有新的连接到来
    void OnConnect(const std::shared_ptr<Socket>& socket) override;

    // 管理会话
    void AddSession(const std::shared_ptr<PlayerSession>& session);
    void RemoveSession(int32_t connid);
    std::shared_ptr<PlayerSession> GetSession(uint32_t connid);

    // 停止监听  并关闭所有会话
    void Stop() override;

private:
    // connid -> PlayerSession
    std::map<uint32_t, std::shared_ptr<PlayerSession>> _sessions;
    Locker _sessions_lock;
};

#endif
