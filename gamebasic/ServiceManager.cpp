#include "ServiceManager.h"

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