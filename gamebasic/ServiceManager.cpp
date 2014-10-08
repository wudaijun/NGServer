#include "ServiceManager.h"

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