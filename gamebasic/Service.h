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

    // 处理消息队列中的消息 返回值代表消息队列是否还有新的消息
    bool Receive();

    // 接口 处理各类消息 返回true代表消息将由框架删除 返回false自行管理该消息
    virtual bool ProcessMsg(Message* msg);
    virtual bool ProcessMsg(TimerMessage* msg);
    virtual bool ProcessMsg(UserMessage* msg);
    virtual void ProcessMsg(CycleMessage* msg);
    virtual bool ProcessMsg(InsideMessage* msg);

private:
    // 处理单条消息 返回值代表该消息是否需要被删除 
    bool ReceiveMsg(Message* msg);

private:
    MessageQueue<Message*> _msgqueue;
};


typedef std::shared_ptr<Service> ServicePtr;

#endif