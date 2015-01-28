#ifndef __NGSERVER_PLAYER_H_INCLUDE__
#define __NGSERVER_PLAYER_H_INCLUDE__

#include "gamebasic/NetHead.h"
#include "protocol/protocol.h"
#include "PlayerSession.h"
#include "../Service/MapService.h"

/*
*   Player ����һ�����  ������Ҿ������ݺ��߼�����
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

    // ���ݽ���
    int32_t Decode(const char* data, size_t len);

    // ��Ϣ��Ӧ
public:
    void OnDBLoadCompleted();

    // ��Ϣ����
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
    // ����ID
    inline void SetConnId(int32_t connid){ _conn_id = connid; }
    inline int32_t GetConnId() const{ return _conn_id; }

    // ���ID
    inline int64_t GetPlayerId() const { return _playerid; }

    void SetMapService(std::shared_ptr<MapService> mapservice)
    {
        _mapservice = mapservice;
    }

    // �����ߴ���
    void Close();

public:
    string _name;

private:
    std::shared_ptr<PlayerSession> _session;// ����Ự
    int32_t _conn_id;   // ����ID
    int64_t _playerid;  // ���ID
    std::shared_ptr<MapService> _mapservice; // ����MapService

#pragma region �ص�����
public:

#pragma endregion

};

typedef std::shared_ptr<Player> PlayerPtr;


#endif
