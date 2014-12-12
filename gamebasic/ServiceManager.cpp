#include "ServiceManager.h"

 // ��̬��Ա����
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

// ������Ϣ��ָ��Service msg�Ĺ���Ȩ��ת�� �����߲����ٹ���msg���ͷ�����
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
// ��ָ���������Ͷ�ʱ��Ϣ
bool ServiceManager::Send(int32_t sid, TimerMessage* msg, int64_t time)
{
    // У��ʱ�� 2ms
    const int64_t adjust = 2;
    if (time < 2)
    { // С��У��ʱ�� ֱ�ӷ���
        return Send(sid, msg);
    }

    // �������Ϣ�ķ���ʱ��
    int64_t nexttime = Service::SteadyNow() + (time - adjust) * 10000;
    // ����ʱ��Ϣ���붨ʱ������
    ServiceTimer* timermsg = new ServiceTimer(nexttime, sid, msg);
    PushTimerMessage(timermsg);
    return true;
}

// ���һ��ָ��Service ����Service sid ʧ���򷵻�0 
// �����Service _sidΪ0 ��Ϊ��Ѱ�Ҳ���һ���µ�sid
// ����Service����_sidָ��λ��  
uint16_t ServiceManager::RegisterService(ServicePtr s)
{
    if (s != nullptr)
    {
        AutoLocker aLock(&_locker);

        // û��ָ��λ�� ��Ϊ��Ѱ��һ��λ��
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
        // ����ִ��_ready_services�е�Service
        while (_runing)
        {
            ServicePtr sptr = _ready_services.Pop();
            if (sptr != nullptr)
            {
                if (sptr->Receive()) 
                {// ���ִ����ɺ� ����δ������Ϣ
                    // ����Ͷ�ݵ���ִ�ж���
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

// ѹ��ѭ����Ϣ
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
// ִ��ѭ����Ϣ
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

        // ׼����һ�μ�ʱ
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

        // �ַ���ʱ��Ϣ
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
            // �ϸ���ʱ��Ϣ��δ������� ��һ�εĶ�ʱ��Ϣ�Ѿ�����  ���ƿ�֡
        }
    }
}

// ѹ�붨ʱ��Ϣ
void ServiceManager::PushTimerMessage(ServiceTimer* msg)
{
    AutoLocker aLock(&_timermsg_lock);

    _timermsg_queue.push(msg);
}

// ִ�ж�ʱ��Ϣ
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

// ��ʼ����
// threadNum��ָ�����е��߳�����
// ���ServiceManager�Ѿ��������� ����ԭ���̻߳��������¿�threadNum���߳�
void ServiceManager::Start(int threadNum)
{
    AutoLocker aLock(&_locker);
    if (_runing == false)
    {   // ServiceManager��Ҫһ��TimerThread���ڹ������ж�ʱ��Ϣ
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

// ֹͣ����
void ServiceManager::StopThread()
{
    AutoLocker aLock(&_locker);
 
    _runing = false;

    // ֹͣ�����߳�
    for (auto t : _threads)
    {
        t->join();
        delete t;
    }
}

void ServiceManager::Stop()
{
    // ֹͣ�߳�
    StopThread();
}

