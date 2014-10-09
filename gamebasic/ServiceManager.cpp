#include "ServiceManager.h"
#include <thread>
#include <chrono>

// 发送消息到指定Service msg的管理权将转交 调用者不需再关心msg的释放问题
bool ServiceManager::Send(int32_t sid, Message* msg)
{
    if (sid < kMaxServiceNum)
    {
        ServicePtr sptr = _serviceMap[sid];
        if (sptr != nullptr)
        {
            if (sptr->PushMsg(msg))
            {
                PushService(sptr);
            }
            return true;
        }
    }
    delete msg;
    return false;
}


bool ServiceManager::PushService(ServicePtr& sptr)
{
    return _ready_services.Push(sptr) > 0;
}

void ServiceManager::ExecThread()
{
    try
    {
        // 不断执行_ready_services中的Service
        while (_runing)
        {
            ServicePtr sptr = _ready_services.Pop();
            if (sptr != nullptr)
            {
                if (sptr->Receive()) 
                {// 如果执行完成后 还有未处理消息
                    // 重新投递到待执行队列
                    PushService(sptr);
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
    catch (std::runtime_error& err)
    {
        std::cerr << "runing thread catch one exception : " << err.what() << std::endl;
    }
}

void ServiceManager::TimerThread()
{

}

// 开始运行
// threadNum：指定运行的线程数量
// 如果ServiceManager已经在运行中 则在原有线程基础上再新开threadNum个线程
void ServiceManager::Start(int threadNum)
{
    AutoLocker aLock(&_locker);
    if (_runing == false)
    {   // ServiceManager需要一个TimerThread用于管理所有定时消息
        std::thread* t = new std::thread(TimerThread);
        _threads.push_back(t);
        _runing = true;
    }
    
    for (int i = 0; i < threadNum; i++)
    {
        std::thread* t = new std::thread(ExecThread);
        _threads.push_back(t);
    }
}

// 停止运行
void ServiceManager::StopThread()
{
    AutoLocker aLock(&_locker);
    // 停止所有线程
    for (auto t : _threads)
    {
        t->join();
        delete t;
    }
    _runing = false;
}

void ServiceManager::Stop()
{
    // 停止线程
    StopThread();
}

