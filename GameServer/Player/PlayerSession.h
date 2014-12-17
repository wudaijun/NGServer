#ifndef __NGSERVER_PLAYERSESSION_H_INCLUDE__
#define __NGSERVER_PLAYERSESSION_H_INCLUDE__

#include "gamebasic/Session.h"
#include "protocol/protocol.h"
#include "protocol/msgid.h"

using namespace NGServer::protocol;

enum SessionState
{
    kSessionState_None,     // 未知状态
    kSessionState_Logging,  // 正在登录
    kSessionState_Success,  // 登录成功
    kSessionState_Failed,  // 登录失败
};

class Player;

class PlayerSession : public Session
{
public:
    PlayerSession(const std::shared_ptr<Socket> socket, int32_t conn_id);
    ~PlayerSession();

    // 会话状态
    SessionState GetState()const { return _state; }
    void SetState(SessionState state) { _state = state; }
    bool IsState(SessionState state) { return _state == state;  }

    // 玩家指针
    std::shared_ptr<Player> GetPlayerToken()const { return _playerToken.lock(); }
    void SetPlayerToken(std::shared_ptr<Player>& token){ _playerToken = token; }

    // 服务ID
    int32_t GetSid() const { return _sid;  }
    void SetSid(int32_t sid) { _sid = sid; }

    // 连接ID
    int32_t GetConnId() const { return _id; }
    void SetConnId(int32_t connid) { _id = connid; }

    // 登录用户名
    std::string GetOwner()const { return _owner;  }
    void SetOwner(const std::string& owner) { _owner = owner;  }

    // 网络数据解码
    int32_t Decode(const char* data, int32_t len);

    // 编码消息并发送消息
    // 该接口需 MsgT包含一静态成员 msgid 标识其消息ID
    template<typename MsgT>
    bool SendMsg(MsgId msgid, MsgT& t)
    {
        char buf[kBufferSize];
        ProtocolWriter encoder(buf, kBufferSize);

#ifdef _DEBUG
        assert(MsgT::msgid == msgid);

        uint16_t msgLen = ProtocolWriter::kHeadLen + ProtocolWriter::kMsgIdLen;
        msgLen += GetMsgSize(t);

        if (msgLen < kBufferSize)
        {
            uint16_t writeLen = encoder.EncodeMsg(t);
            assert(writeLen == msgLen);
            return SendAsync(buf, writeLen);
        }
        return false;
#else
        uint16_t writeLen = encoder.EncodeMsg(t);
        return SendAsync(buf, writeLen);
#endif
    }

private:
    int32_t _sid = 0;       // 所属服务ID
    std::weak_ptr<Player> _playerToken; // 玩家指针
    SessionState _state = kSessionState_None;   // 会话状态
    std::string _owner;       // 登录用户名
};

typedef std::shared_ptr<PlayerSession> PlayerSessionPtr;

struct SS_PlayerLogin
{
    SSMSGID(kSS_PlayerLogin);
    PlayerSessionPtr session;
    D2S_PlayerLoginResult login_info;
};

#endif