#include "PlayerSession.h"
#include "gamebasic/ServiceManager.h"
#include "gamebasic/Message.h"

#include <iostream>

PlayerSession::PlayerSession(const std::shared_ptr<Socket> socket, int32_t conn_id)
    :Session(socket, conn_id)
{
    //std::cout << "PlayerSession" << std::endl;
}

PlayerSession::~PlayerSession()
{
    // std::cout << "~PlayerSession" << std::endl;
}

// 解码原始数据 将数据由网络IO转向逻辑层
// 返回值： >=0 : 缓冲区经处理之后剩余的字节数
//          <0 : 服务器端主动断线(对应Service断线或者发送消息到逻辑层失败)
int32_t PlayerSession::Decode(const char* data, int32_t len)
{
    PlayerSessionPtr _this = std::dynamic_pointer_cast<PlayerSession>(this->shared_from_this());

    // 客户端断线
    if (data == nullptr || len == 0)
    {
        // 通知业务逻辑层 处理下线逻辑
        Message* msg = new UserMessageT<PlayerSessionPtr>(data, len, _this);
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
        Message* msg = new UserMessageT<PlayerSessionPtr>(buff, msgLen, _this);
        if (!ServiceManager::Send(_sid, msg))
        {
            // 服务器主动断线
            return -1;
        }

        remainLen -= msgLen;
        buff += msgLen;
    }
    return remainLen;

}

