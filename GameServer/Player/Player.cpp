#include "Player.h"
#include "../../gamebasic/Message.h"
#include "../../gamebasic/ServiceManager.h"

int32_t Player::Decode(const char* data, size_t len)
{
    // 客户端断线
    if (data == nullptr || len == 0)
    {
#ifdef _DEBUG
        std::cout << " ~~ Player[" << _conn_id << "] LogOut" << std::endl;
#endif 
        return 0;
    }

    // 消息的解码
    string str(data, len);
#ifdef _DEBUG
    std::cout << " -- Player[" << _conn_id << "]: " << str << std::endl;
#endif

    // 发送到默认Service
    UserMessageT<Player>* msg = new UserMessageT<Player>(data, len, this);
    if (!ServiceManager::Send(_default_sid, msg))
    {
        // 服务器主动断线
        return -1;
    }

    return 0;
}