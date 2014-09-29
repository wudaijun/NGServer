#ifndef __NGSERVER_SERVICE_H_INLCUDE__
#define __NGSERVER_SERVICE_H_INLCUDE__

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Message.h"
#include "MessageQueue.h"

class Service : public boost::enable_shared_from_this < Service > 
{
public:
    virtual bool Init();

    // ������Ϣ�����е���Ϣ ����ֵ������Ϣ�����Ƿ����µ���Ϣ
    bool Receive();

    // �ӿ� ���������Ϣ ����true������Ϣ���ɿ��ɾ�� ����false���й������Ϣ
    virtual bool ProcessMsg(Message* msg);
    virtual bool ProcessMsg(TimerMessage* msg);
    virtual bool ProcessMsg(UserMessage* msg);
    virtual void ProcessMsg(CycleMessage* msg);
    virtual bool ProcessMsg(InsideMessage* msg);

private:
    // ��������Ϣ ����ֵ�������Ϣ�Ƿ���Ҫ��ɾ�� 
    bool ReceiveMsg(Message* msg);

private:
    MessageQueue<Message*> _msgqueue;
};


typedef std::shared_ptr<Service> ServicePtr;

#endif