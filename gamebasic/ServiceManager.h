#ifndef __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#define __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#include "NetHead.h"
#include "Service.h"
#include "Message.h"
#include "ServiceQueue.h"
/*
*       File: ServiceManager.h
*    Created: 2014.09.25 凌晨. by wudaijun 
*/
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

    // 向指定Service发送消息
    static bool Send(int32_t sid, Message* msg);

    // 将Service加入待执行队列
    static bool PushService(ServicePtr& sptr);
private:
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
};

#endif