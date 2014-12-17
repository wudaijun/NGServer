#ifndef __NGSERVER_PLAYERSESSION_H_INCLUDE__
#define __NGSERVER_PLAYERSESSION_H_INCLUDE__

#include "gamebasic/Session.h"
#include "protocol/protocol.h"
#include "protocol/msgid.h"

using namespace NGServer::protocol;

enum SessionState
{
    kSessionState_None,     // δ֪״̬
    kSessionState_Logging,  // ���ڵ�¼
    kSessionState_Success,  // ��¼�ɹ�
    kSessionState_Failed,  // ��¼ʧ��
};

class Player;

class PlayerSession : public Session
{
public:
    PlayerSession(const std::shared_ptr<Socket> socket, int32_t conn_id);
    ~PlayerSession();

    // �Ự״̬
    SessionState GetState()const { return _state; }
    void SetState(SessionState state) { _state = state; }
    bool IsState(SessionState state) { return _state == state;  }

    // ���ָ��
    std::shared_ptr<Player> GetPlayerToken()const { return _playerToken.lock(); }
    void SetPlayerToken(std::shared_ptr<Player>& token){ _playerToken = token; }

    // ����ID
    int32_t GetSid() const { return _sid;  }
    void SetSid(int32_t sid) { _sid = sid; }

    // ����ID
    int32_t GetConnId() const { return _id; }
    void SetConnId(int32_t connid) { _id = connid; }

    // ��¼�û���
    std::string GetOwner()const { return _owner;  }
    void SetOwner(const std::string& owner) { _owner = owner;  }

    // �������ݽ���
    int32_t Decode(const char* data, int32_t len);

    // ������Ϣ��������Ϣ
    // �ýӿ��� MsgT����һ��̬��Ա msgid ��ʶ����ϢID
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
    int32_t _sid = 0;       // ��������ID
    std::weak_ptr<Player> _playerToken; // ���ָ��
    SessionState _state = kSessionState_None;   // �Ự״̬
    std::string _owner;       // ��¼�û���
};

typedef std::shared_ptr<PlayerSession> PlayerSessionPtr;

struct SS_PlayerLogin
{
    SSMSGID(kSS_PlayerLogin);
    PlayerSessionPtr session;
    D2S_PlayerLoginResult login_info;
};

#endif