#ifndef __NGSERVER_PLAYER_H_INCLUDE__
#define __NGSERVER_PLAYER_H_INCLUDE__

#include "../../gamebasic/NetHead.h"
#include "../../gamebasic/Session.h"

/*
*   Player 代表一个玩家  负责玩家具体数据和逻辑交互
*
*/

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(SessionPtr session, int32_t defaultSid)
        :_session(session), _sid(defaultSid)
    {
    }

    inline void SetConnId(int32_t connid)
    { 
        _conn_id = connid;  
    }

    inline int32_t GetConnId() const 
    { 
        return _conn_id;  
    }

    inline int32_t SetSid(int32_t sid) 
    { 
        _sid = sid;
    }
    inline int32_t GetSid() const 
    { 
        return _sid; 
    }

public:
    // 数据解码
    int32_t Decode(const char* data, size_t len);

    // 作下线处理
    void Offline();

public:
    string _name;

private:
    SessionPtr _session;// 网络会话
    int32_t _sid;       // 当前服务ID
    int32_t _conn_id;   // 连接ID

#pragma region 回调函数
public:

#pragma endregion

};

typedef std::shared_ptr<Player> PlayerPtr;


#endif