#ifndef __NGSERVER_PLAYER_H_INCLUDE__
#define __NGSERVER_PLAYER_H_INCLUDE__

#include "gamebasic/NetHead.h"
#include "protocol/protocol.h"
#include "PlayerSession.h"
#include "../Service/MapService.h"

/*
*   Player 代表一个玩家  负责玩家具体数据和逻辑交互
*
*/

using namespace NGServer::protocol;

//class MapService;

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(int64_t playerid, std::shared_ptr<PlayerSession> session);
    ~Player();
public:
    inline void SetSession(std::shared_ptr<PlayerSession>& session)
    { 
        _session = session; 
    }
    inline std::shared_ptr<PlayerSession> GetSession() const { return _session; }

    // 数据解码
    int32_t Decode(const char* data, size_t len);

    // 消息响应
public:
    void OnDBLoadCompleted();

    // 消息发送
public:
    template<typename MsgT>
    bool SendToDB(DBMsgId msgid, MsgT& t)
    {
        if (_mapservice)
            return _mapservice->SendToDB(_playerid, msgid, t);
        return false;
    }

    template<typename MsgT>
    bool SendMsg(MsgId msgid, MsgT& t)
    {
        if (_session.get())
            return _session->SendMsg(msgid, t);
        return false;
    }

public:
    // 连接ID
    inline void SetConnId(int32_t connid){ _conn_id = connid; }
    inline int32_t GetConnId() const{ return _conn_id; }

    // 玩家ID
    inline int64_t GetPlayerId() const { return _playerid; }

    void SetMapService(std::shared_ptr<MapService> mapservice)
    {
        _mapservice = mapservice;
    }

    // 作下线处理
    void Close();

public:
    string _name;

private:
    std::shared_ptr<PlayerSession> _session;// 网络会话
    int32_t _conn_id;   // 连接ID
    int64_t _playerid;  // 玩家ID
    std::shared_ptr<MapService> _mapservice; // 所属MapService

#pragma region 回调函数
public:

#pragma endregion

};

typedef std::shared_ptr<Player> PlayerPtr;


#endif
