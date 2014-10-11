#include "Player.h"
#include "../../gamebasic/Message.h"
#include "../../gamebasic/ServiceManager.h"

int32_t Player::Decode(const char* data, size_t len)
{
    // �ͻ��˶���
    if (data == nullptr || len == 0)
    {
#ifdef _DEBUG
        std::cout << " ~~ Player[" << _conn_id << "] LogOut" << std::endl;
#endif 
        return 0;
    }

    // ��Ϣ�Ľ���
    string str(data, len);
#ifdef _DEBUG
    std::cout << " -- Player[" << _conn_id << "]: " << str << std::endl;
#endif

    // ���͵�Ĭ��Service
    UserMessageT<Player>* msg = new UserMessageT<Player>(data, len, this);
    if (!ServiceManager::Send(_default_sid, msg))
    {
        // ��������������
        return -1;
    }

    return 0;
}