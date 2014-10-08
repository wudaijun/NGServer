#ifndef __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#define __NGSERVER_SERVICEMANAGER_H_INCLUDE__
#include "NetHead.h"
#include "Service.h"
#include "Message.h"
#include "ServiceQueue.h"
/*
*       File: ServiceManager.h
*    Created: 2014.09.25 �賿. by wudaijun 
*/
// ��̬�� �������е�Service
class ServiceManager
{
public:
    static const int32_t kMaxServiceNum = 256 * 256;

    static int32_t GetDefaultSid()
    {
        return 0;
    }

public:

    // ��ָ��Service������Ϣ
    static bool Send(int32_t sid, Message* msg);

    // ��Service�����ִ�ж���
    static bool PushService(ServicePtr& sptr);
private:
    static ServicePtr _serviceMap[kMaxServiceNum];
    static ServiceQueue _ready_services;
};

#endif