#include "ServiceManager.h"
#include <thread>
#include <chrono>

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

}

// ��ʼ����
// threadNum��ָ�����е��߳�����
// ���ServiceManager�Ѿ��������� ����ԭ���̻߳��������¿�threadNum���߳�
void ServiceManager::Start(int threadNum)
{
    AutoLocker aLock(&_locker);
    if (_runing == false)
    {   // ServiceManager��Ҫһ��TimerThread���ڹ������ж�ʱ��Ϣ
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

// ֹͣ����
void ServiceManager::StopThread()
{
    AutoLocker aLock(&_locker);
    // ֹͣ�����߳�
    for (auto t : _threads)
    {
        t->join();
        delete t;
    }
    _runing = false;
}

void ServiceManager::Stop()
{
    // ֹͣ�߳�
    StopThread();
}

