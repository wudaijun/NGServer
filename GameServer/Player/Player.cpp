#include "Player.h"
#include "../../gamenet/Message.h"
#include "../../gamenet/ServiceManager.h"

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
    UserMessageT<PlayerPtr>* msg = new UserMessageT<PlayerPtr>(data, len, this->shared_from_this());
    if (!ServiceManager::Send(_sid, msg))
    {
        // ��������������
        return -1;
    }

    return 0;
}

void Player::Offline()
{
    
}