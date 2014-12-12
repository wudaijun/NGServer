#ifndef __NGSERVER_SessionManager_H_INCLUDE__
#define __NGSERVER_SessionManager_H_INCLUDE__

#include "gamebasic/AsyncTcpListener.h"
#include <map>
#include "common/Locker.h"

/*
*   �ļ�����SessionManager.h ����SessionManager��
*   SessionManager�ࣺ
*       �������еĻỰPlayerSession ��ʵ������һ��AsyncTcpListener����Ҫ����
*       1. �����µĿͻ������� ������PlayerSession �������Ự
*       2. �������л�Ự
*
*   ����SessionManager��������Service�ϵ���һỰ��Ϊ����Service�������
*   ����������̰߳�ȫ
*/

class PlayerSession;

class SessionManager : public AsyncTcpListener
{
public:
    SessionManager(IOService* ioservice)
        :AsyncTcpListener(ioservice){}

    // ���µ����ӵ���
    void OnConnect(const std::shared_ptr<Socket>& socket) override;

    // ����Ự
    void AddSession(const std::shared_ptr<PlayerSession>& session);
    void RemoveSession(int32_t connid);
    std::shared_ptr<PlayerSession> GetSession(uint32_t connid);

    // ֹͣ����  ���ر����лỰ
    void Stop() override;

private:
    // connid -> PlayerSession
    std::map<uint32_t, std::shared_ptr<PlayerSession>> _sessions;
    Locker _sessions_lock;
};

#endif
