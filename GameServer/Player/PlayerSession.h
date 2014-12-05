#ifndef __NGSERVER_PLAYERSESSION_H_INCLUDE__
#define __NGSERVER_PLAYERSESSION_H_INCLUDE__

#include "gamebasic/Session.h"
#include "Player.h"

class PlayerSession : public Session
{
    enum SessionState
    {
        kSessionState_None,     // 未知状态
        kSessionState_Logging,  // 正在登录
        kSessionState_Success,  // 登录成功
        kSessionState_Failed,  // 登录失败
    };

public:
    PlayerSession(const std::shared_ptr<Socket> socket, int32_t conn_id) :
        Session(socket, conn_id){}

    // 会话状态
    SessionState GetState()const { return _state; }
    void SetState(SessionState state) { _state = state; }

    // 玩家指针
    PlayerPtr GetPlayerToken()const { return _playerToken; }
    void SetPlayerToken(PlayerPtr token){ _playerToken = token; }

    // 服务ID
    int32_t GetSid() const { return _sid;  }
    void SetSid(int32_t sid) { _sid = sid; }

    // 连接ID
    int32_t GetConnId() const { return _connid; }
    void SetConnId(int32_t connid) { _connid = connid; }

    // 网络数据解码
    int32_t Decode(const char* data, int32_t len);

private:
    int32_t _sid = 0;       // 所属服务ID
    int32_t _connid = 0;    // 连接ID
    PlayerPtr _playerToken; // 玩家指针
    SessionState _state = kSessionState_None;   // 会话状态
};

typedef std::shared_ptr<PlayerSession> PlayerSessionPtr;

#endif