#include "Player.h"
#include "../../gamebasic/Message.h"
#include "../../gamebasic/ServiceManager.h"
#include "../../protocol/protocol.h"
using namespace NGServer::protocol;

int32_t Player::Decode(const char* data, size_t len)
{
    // �ͻ��˶���
    if (data == nullptr || len == 0)
    {
#ifdef _DEBUG
        std::cout << " ~~ Player[" << _conn_id << "] LogOut" << std::endl;
#endif 
        // ֪ͨҵ���߼��� ���������߼�
        Message* msg = new UserMessageT<PlayerPtr>(data, len, shared_from_this());
        ServiceManager::Send(_sid, msg);
        return 0;
    }

    // ��Ϣ�Ľ��
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

        // ���͵�Service��ܲ�
        Message* msg = new UserMessageT<PlayerPtr>(buff, msgLen, shared_from_this());
        if (!ServiceManager::Send(_sid, msg))
        {
            // ��������������
            return -1;
        }

        remainLen -= msgLen;
        buff += msgLen;
    }
    return remainLen;

}

void Player::Offline()
{
    
}