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
    // �ͻ��˶���
    if (data == nullptr || len == 0)
    {
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
    */
    return 0;
}

void Player::Close()
{
    // �������ָ���໥������
    //_session->SetPlayerToken(std::shared_ptr<Player>(nullptr));
    // �ر�����IO
    _session->DisConnect(false);
}

// ���ݿ�����������
void Player::OnDBLoadCompleted()
{
    // ��ʱ���ҵ�����ݾ��Ѽ��ص��ڴ���
    // ���͸���ϵͳ��Ҫ�ĳ�ʼ��ͬ�����ݵ��ͻ���
    // �磺 _task.LoadTask(); 
    //     _gate.LoadGate();
    // ......

    // ������Ϣ ���߿ͻ��˼��������
    S2C_LoadCompleted msg;
    msg.teststr = "��ӭ������Ϸ����.";
    SendMsg(kS2C_LoadCompleted, msg);

}

