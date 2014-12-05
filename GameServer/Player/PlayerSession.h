#ifndef __NGSERVER_PLAYERSESSION_H_INCLUDE__
#define __NGSERVER_PLAYERSESSION_H_INCLUDE__

#include "gamebasic/Session.h"
#include "Player.h"

class PlayerSession : public Session
{
    enum SessionState
    {
        kSessionState_None,     // δ֪״̬
        kSessionState_Logging,  // ���ڵ�¼
        kSessionState_Success,  // ��¼�ɹ�
        kSessionState_Failed,  // ��¼ʧ��
    };

public:
    PlayerSession(const std::shared_ptr<Socket> socket, int32_t conn_id) :
        Session(socket, conn_id){}

    // �Ự״̬
    SessionState GetState()const { return _state; }
    void SetState(SessionState state) { _state = state; }

    // ���ָ��
    PlayerPtr GetPlayerToken()const { return _playerToken; }
    void SetPlayerToken(PlayerPtr token){ _playerToken = token; }

    // ����ID
    int32_t GetSid() const { return _sid;  }
    void SetSid(int32_t sid) { _sid = sid; }

    // ����ID
    int32_t GetConnId() const { return _connid; }
    void SetConnId(int32_t connid) { _connid = connid; }

    // �������ݽ���
    int32_t Decode(const char* data, int32_t len);

private:
    int32_t _sid = 0;       // ��������ID
    int32_t _connid = 0;    // ����ID
    PlayerPtr _playerToken; // ���ָ��
    SessionState _state = kSessionState_None;   // �Ự״̬
};

typedef std::shared_ptr<PlayerSession> PlayerSessionPtr;

#endif