#include "PlayerSession.h"
#include "gamebasic/ServiceManager.h"
#include "gamebasic/Message.h"

// ����ԭʼ���� ������������IOת���߼���
int32_t PlayerSession::Decode(const char* data, int32_t len)
{
    PlayerSessionPtr _this = std::dynamic_pointer_cast<PlayerSession>(this->shared_from_this());

    // �ͻ��˶���
    if (data == nullptr || len == 0)
    {
        // ֪ͨҵ���߼��� ���������߼�
        Message* msg = new UserMessageT<PlayerSessionPtr>(data, len, _this);
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
        Message* msg = new UserMessageT<PlayerSessionPtr>(buff, msgLen, _this);
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

