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
*    Created: 2014.09.25 凌晨. by wudaijun 
*/

struct ServiceTimer
{
    ServiceTimer(std::chrono::steady_clock::rep nexttime, int32_t sid, TimerMessage* msg) :
        _nexttime(nexttime), _sid(sid), _msg(msg){}

    std::chrono::steady_clock::rep _nexttime;
    int32_t _sid;
    TimerMessage* _msg;
};
// 静态类 管理所有的Service
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

    // 创建新的服务
public:
    // 创建各种类型的服务
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
    // 将服务放到Manager中
    uint16_t RegisterService(ServicePtr s);

public:
    
    // 开始运行 指定运行的线程数
    static void Start(int threadNum);

    // 停止运行
    static void StopThread();
    // 停止并清空整个服务管理器
    static void Stop();

    // 向指定Service发送消息
    static bool Send(int32_t sid, Message* msg);

    // 向指定Service发送定时消息 也就是time毫秒之后再发送msg到sid对应Service
    static bool Send(int32_t sid, TimerMessage* msg, int64_t time);

    // 将Service加入待执行队列
    static bool PushService(ServicePtr& sptr);

public:

    // 线程入口函数 执行待处理的Service  可多个线程执行该函数
    static void ExecThread();
    // 线程入口函数 执行定时功能 管理定时消息和循环消息
    static void TimerThread();

    // 管理循环消息 对于已经触发的消息 将其推送到对应服务
    static void ExecCycleMessage();
    static CycleMessage* PushCycleMessage(uint16_t sid, int32_t proid);
    static void PushCycleMessage(CycleMessage* msg);
    
    // 管理定时消息
    static void ExecTimerMessage();
    static void PushTimerMessage(ServiceTimer* msg);

private:
    static int32_t _serviceCount;
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
    static bool _runing;
    static std::vector<std::thread*> _threads;
    static Locker _locker;

    // 定时消息比较器
    template<typename T>
    struct TimerComparator
    {
        bool operator()(T* a, T* b)
        {
            return a->_nexttime > b->_nexttime;
        }
    };
    // 定时消息队列 统一管理
    static Locker _cyclemsg_lock;
    static std::priority_queue<CycleMessage*, deque<CycleMessage*>, TimerComparator<CycleMessage> > _cyclemsg_queue;
    static Locker _timermsg_lock;
    static std::priority_queue<ServiceTimer*, deque<ServiceTimer*>, TimerComparator<ServiceTimer> > _timermsg_queue;
};

#endif