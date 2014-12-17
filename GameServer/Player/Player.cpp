#include "Player.h"
#include "../../gamebasic/Message.h"
#include "../../gamebasic/ServiceManager.h"
#include "../../protocol/protocol.h"
using namespace NGServer::protocol;

#include <iostream>
Player::~Player()
{
    std::cout << "--- Player[" << _playerid << "] Logout." << std::endl;
}

Player::Player(int64_t playerid, std::shared_ptr<PlayerSession> session)
    :_session(session), _playerid(playerid)
{
    std::cout << "+++ Player[" << _playerid << "] Login." << std::endl;
}

int32_t Player::Decode(const char* data, size_t len)
{
    /*
    // 客户端断线
    if (data == nullptr || len == 0)
    {
        // 通知业务逻辑层 处理下线逻辑
        Message* msg = new UserMessageT<PlayerPtr>(data, len, shared_from_this());
        ServiceManager::Send(_sid, msg);
        return 0;
    }

    // 消息的解包
    const char* buff = data;
    size_t remainLen = len;
    static const uint16_t headLen = ProtocolStream::kHeadLen + ProtocolStream::kMsgIdLen;
    while (remainLen > headLen)
    {
        int32_t msgLen = std::max(headLen, *((uint16_t*)buff));
        if (remainLen < msgLen)
        {
            break;
        }

        // 发送到Service框架层
        Message* msg = new UserMessageT<PlayerPtr>(buff, msgLen, shared_from_this());
        if (!ServiceManager::Send(_sid, msg))
        {
            // 服务器主动断线
            return -1;
        }

        remainLen -= msgLen;
        buff += msgLen;
    }
    return remainLen;
    */
    return 0;
}

void Player::Close()
{
    // 解除智能指针相互引用链
    //_session->SetPlayerToken(std::shared_ptr<Player>(nullptr));
    // 关闭网络IO
    _session->DisConnect(false);
}

// 数据库加载数据完成
void Player::OnDBLoadCompleted()
{
    // 此时玩家业务数据均已加载到内存中
    // 发送各子系统必要的初始化同步数据到客户端
    // 如： _task.LoadTask(); 
    //     _gate.LoadGate();
    // ......

    // 测试消息 告诉客户端加载已完成
    S2C_LoadCompleted msg;
    msg.teststr = "欢迎来到游戏世界.";
    SendMsg(kS2C_LoadCompleted, msg);

}

