#ifndef __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#define __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#include "NetHead.h"
#include "Service.h"
#include "Message.h"
#include "ServiceQueue.h"
#include <queue>
#include <chrono>

/*
*       File: ServiceManager.h
*    Created: 2014.09.25 �賿. by wudaijun 
*/

struct ServiceTimer
{
    ServiceTimer(std::chrono::steady_clock::rep nexttime, int32_t sid, TimerMessage* msg) :
        _nexttime(nexttime), _sid(sid), _msg(msg){}

    std::chrono::steady_clock::rep _nexttime;
    int32_t _sid;
    TimerMessage* _msg;
};
// ��̬�� �������е�Service
class ServiceManager
{
public:
    static const int32_t kMaxServiceNum = 256 * 256;

    static int32_t GetDefaultSid()
    {
        return 0;
    }

public:

    // ��ʼ���� ָ�����е��߳���
    void Start(int threadNum);

    // ֹͣ����
    void StopThread();
    // ֹͣ������������������
    void Stop();

    // ��ָ��Service������Ϣ
    static bool Send(int32_t sid, Message* msg);

    // ��Service�����ִ�ж���
    static bool PushService(ServicePtr& sptr);

public:

    // �߳���ں��� ִ�д������Service  �ɶ���߳�ִ�иú���
    void ExecThread();
    // �߳���ں��� ִ�ж�ʱ���� ����ʱ��Ϣ��ѭ����Ϣ
    void TimerThread();

private:
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
    static bool _runing = false;
    static std::vector<std::thread*> _threads;
    static Locker _locker;

    // ��ʱ��Ϣ�Ƚ���
    template<typename T>
    struct TimerComparator
    {
        bool operator(T* a, T* b)
        {
            return a->_nexttime > b->_nexttime;
        }
    };
    // ��ʱ��Ϣ���� ͳһ����
    std::priority_queue<CycleMessage*, deque<CycleMessage*>, TimerComparator<CycleMessage> > _cyclemsg_queue;
    std::priority_queue<ServiceTimer*, deque<ServiceTimer*>, TimerComparator<ServiceTimer> > _timermsg_queue;
};

#endif