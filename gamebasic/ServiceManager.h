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
    static const int32_t kMaxServiceNum = 256 * 256;

    static int32_t GetDefaultSid()
    {
        return 0;
    }

public:

    // 开始运行 指定运行的线程数
    void Start(int threadNum);

    // 停止运行
    void StopThread();
    // 停止并清空整个服务管理器
    void Stop();

    // 向指定Service发送消息
    static bool Send(int32_t sid, Message* msg);

    // 将Service加入待执行队列
    static bool PushService(ServicePtr& sptr);

public:

    // 线程入口函数 执行待处理的Service  可多个线程执行该函数
    void ExecThread();
    // 线程入口函数 执行定时功能 管理定时消息和循环消息
    void TimerThread();

private:
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
    static bool _runing = false;
    static std::vector<std::thread*> _threads;
    static Locker _locker;

    // 定时消息比较器
    template<typename T>
    struct TimerComparator
    {
        bool operator(T* a, T* b)
        {
            return a->_nexttime > b->_nexttime;
        }
    };
    // 定时消息队列 统一管理
    std::priority_queue<CycleMessage*, deque<CycleMessage*>, TimerComparator<CycleMessage> > _cyclemsg_queue;
    std::priority_queue<ServiceTimer*, deque<ServiceTimer*>, TimerComparator<ServiceTimer> > _timermsg_queue;
};

#endif