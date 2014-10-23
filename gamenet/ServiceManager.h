#ifndef __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#define __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#include "NetHead.h"
#include "Service.h"
#include "Message.h"
#include "ServiceQueue.h"
#include <queue>
#include <chrono>
#include <thread>

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
    ServiceManager() = delete;
    ServiceManager(const ServiceManager& rhs) = delete;
    ServiceManager& operator = (const ServiceManager& rhs) = delete;
    ~ServiceManager() = delete;

public:
    static const int32_t kMaxServiceNum = 256 * 256;

    static int32_t GetDefaultSid()
    {
        return 1;
    }

    // �����µķ���
public:
    // �����������͵ķ���
    template < typename S >
    std::shared_ptr<S> Spawn(uint16_t sid=0)
    {
        std::shared_ptr<S> s = std::make_shared<S>(sid);
        if (s != nullptr && RegisterService(std::dynamic_pointer_cast<Service>(s)) != 0)
        {
            return s;
        }
        delete s;
        return nullptr;
    }
    // ������ŵ�Manager��
    uint16_t RegisterService(ServicePtr s);

public:
    
    // ��ʼ���� ָ�����е��߳���
    static void Start(int threadNum);

    // ֹͣ����
    static void StopThread();
    // ֹͣ������������������
    static void Stop();

    // ��ָ��Service������Ϣ
    static bool Send(int32_t sid, Message* msg);

    // ��ָ��Service���Ͷ�ʱ��Ϣ Ҳ����time����֮���ٷ���msg��sid��ӦService
    static bool Send(int32_t sid, TimerMessage* msg, int64_t time);

    // ��Service�����ִ�ж���
    static bool PushService(ServicePtr& sptr);

public:

    // �߳���ں��� ִ�д������Service  �ɶ���߳�ִ�иú���
    static void ExecThread();
    // �߳���ں��� ִ�ж�ʱ���� ����ʱ��Ϣ��ѭ����Ϣ
    static void TimerThread();

    // ����ѭ����Ϣ �����Ѿ���������Ϣ �������͵���Ӧ����
    static void ExecCycleMessage();
    static CycleMessage* PushCycleMessage(uint16_t sid, int32_t proid);
    static void PushCycleMessage(CycleMessage* msg);
    
    // ����ʱ��Ϣ
    static void ExecTimerMessage();
    static void PushTimerMessage(ServiceTimer* msg);

private:
    static int32_t _serviceCount;
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
    static bool _runing;
    static std::vector<std::thread*> _threads;
    static Locker _locker;

    // ��ʱ��Ϣ�Ƚ���
    template<typename T>
    struct TimerComparator
    {
        bool operator()(T* a, T* b)
        {
            return a->_nexttime > b->_nexttime;
        }
    };
    // ��ʱ��Ϣ���� ͳһ����
    static Locker _cyclemsg_lock;
    static std::priority_queue<CycleMessage*, deque<CycleMessage*>, TimerComparator<CycleMessage> > _cyclemsg_queue;
    static Locker _timermsg_lock;
    static std::priority_queue<ServiceTimer*, deque<ServiceTimer*>, TimerComparator<ServiceTimer> > _timermsg_queue;
};

#endif