#include "ServiceManager.h"

 // 静态成员定义
 ServicePtr ServiceManager::_serviceMap[kMaxServiceNum];
 int32_t ServiceManager::_serviceCount = 0;
 ServiceQueue ServiceManager::_ready_services;
 bool ServiceManager::_runing;
 std::vector<std::thread*> ServiceManager::_threads;
 Locker ServiceManager::_locker;

 Locker ServiceManager::_cyclemsg_lock;
 std::priority_queue<CycleMessage*, deque<CycleMessage*>, ServiceManager::TimerComparator<CycleMessage> > ServiceManager::_cyclemsg_queue;
 Locker ServiceManager::_timermsg_lock;
 std::priority_queue<ServiceTimer*, deque<ServiceTimer*>, ServiceManager::TimerComparator<ServiceTimer> > ServiceManager::_timermsg_queue;


CycleMessage* CycleMessage::Create(ServicePtr sptr, int32_t proid)
{
    return new CycleMessage(sptr, Service::SteadyNow() + proid, proid);
}

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
// 向指定服务推送定时消息
bool ServiceManager::Send(int32_t sid, TimerMessage* msg, int64_t time)
{
    // 校正时间 2ms
    const int64_t adjust = 2;
    if (time < 2)
    { // 小于校正时间 直接发送
        return Send(sid, msg);
    }

    // 计算该消息的发送时间
    int64_t nexttime = Service::SteadyNow() + (time - adjust) * 10000;
    // 将定时消息放入定时器队列
    ServiceTimer* timermsg = new ServiceTimer(nexttime, sid, msg);
    PushTimerMessage(timermsg);
    return true;
}

// 添加一个指定Service 返回Service sid 失败则返回0 
// 如果该Service _sid为0 则为其寻找并赋一个新的sid
// 否则将Service放入_sid指定位置  
uint16_t ServiceManager::RegisterService(ServicePtr s)
{
    if (s != nullptr)
    {
        AutoLocker aLock(&_locker);

        // 没有指定位置 则为其寻找一个位置
        if (s->GetSid() == 0)
        {
            for (int32_t i = 1; i < kMaxServiceNum; i++)
            {
                if (_serviceMap[i] == nullptr)
                {
                    s->SetSid(i);
                    break;
                }
            }
        }

        int32_t sid = s->GetSid();
        if (sid > 0 && sid < kMaxServiceNum && _serviceMap[sid] == nullptr)
        {
            if (s->Init())
            {
                _serviceCount++;
                _serviceMap[sid] = s;
                return sid;
            }
        }
    }
    return 0;
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
    try
    {
        while (_runing)
        {
            ExecCycleMessage();
            ExecTimerMessage();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    catch (std::runtime_error& err)
    {
        std::cerr << "timer thread catch one exception :" << err.what() << std::endl;
    }
}

// 压入循环消息
CycleMessage* ServiceManager::PushCycleMessage(uint16_t sid, int32_t proid)
{
    AutoLocker aLock(&_cyclemsg_lock);

    ServicePtr sptr = _serviceMap[sid];
    if (sptr != nullptr)
    {
        CycleMessage* msg = CycleMessage::Create(sptr, proid);
        _cyclemsg_queue.push(msg);
        return msg;
    }
    return nullptr;
}

void ServiceManager::PushCycleMessage(CycleMessage* msg)
{
    AutoLocker aLock(&_cyclemsg_lock);

    _cyclemsg_queue.push(msg);
}
// 执行循环消息
void ServiceManager::ExecCycleMessage()
{
    if (_cyclemsg_queue.empty())
    {
        return;
    }

    AutoLocker aLock(&_cyclemsg_lock);
    auto now = Service::SteadyNow();
    while (!_cyclemsg_queue.empty())
    {
        auto msg = _cyclemsg_queue.top();
        if (msg->_nexttime > now)
        {
            break;
        }

        _cyclemsg_queue.pop();

        // 准备下一次计时
        if (msg->ReadyNextTime())
        {
            _cyclemsg_queue.push(msg);
        }
        else
        {
            delete msg;
            continue;
        }

        cout << " cycle timer . " << endl;

        // 分发定时消息
        if (msg->TryLock())
        {
            auto sptr = msg->_service.lock();
            if (sptr != nullptr && sptr->PushMsg(msg))
            {
                PushService(sptr);
            }
        }
        else
        {
            // 上个定时消息还未处理完成 下一次的定时消息已经触发  类似卡帧
        }
    }
}

// 压入定时消息
void ServiceManager::PushTimerMessage(ServiceTimer* msg)
{
    AutoLocker aLock(&_timermsg_lock);

    _timermsg_queue.push(msg);
}

// 执行定时消息
void ServiceManager::ExecTimerMessage()
{
    if (_timermsg_queue.empty())
    {
        return;
    }

    if (_timermsg_lock.TryLock(256))
    {
        auto now = Service::SteadyNow();
        while (!_timermsg_queue.empty())
        {
            auto msg = _timermsg_queue.top();

            if (msg && msg->_nexttime > now)
            {
                break;
            }

            _timermsg_queue.pop();

            cout << (const char*)msg->_msg->GetData() << endl;

            Send(msg->_sid, msg->_msg);
            delete msg;
        }
        _timermsg_lock.UnLock();
    }
}

// 开始运行
// threadNum：指定运行的线程数量
// 如果ServiceManager已经在运行中 则在原有线程基础上再新开threadNum个线程
void ServiceManager::Start(int threadNum)
{
    AutoLocker aLock(&_locker);
    if (_runing == false)
    {   // ServiceManager需要一个TimerThread用于管理所有定时消息
        _runing = true;
        std::thread* t = new std::thread(TimerThread);
        _threads.push_back(t);
        
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
 
    _runing = false;

    // 停止所有线程
    for (auto t : _threads)
    {
        t->join();
        delete t;
    }
}

void ServiceManager::Stop()
{
    // 停止线程
    StopThread();
}

