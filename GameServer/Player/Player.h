#ifndef __NGSERVER_PLAYER_H_INCLUDE__
#define __NGSERVER_PLAYER_H_INCLUDE__

#include "../../gamebasic/NetHead.h"
#include "../../gamebasic/Session.h"

/*
*   �ļ����� Player.h ����Player��
*   Player�ࣺ ����һ����ң�������ҵĸ����߼�
*/

class Player : public std::enable_shared_from_this<Player>
{
public:
    Player(SessionPtr session, int32_t defaultSid)
        :_session(session), _default_sid(defaultSid)
    {
    }

    int32_t Decode(const char* data, size_t len);
    
    void SetConnId(int32_t connid){ _conn_id = connid;  }
    int32_t GetConnId() const { return _conn_id;  }
private:
    SessionPtr _session;
    int32_t _default_sid;
    int32_t _conn_id;
};

typedef std::shared_ptr<Player> PlayerPtr;


#endif