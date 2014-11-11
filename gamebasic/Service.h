#ifndef __NGSERVER_SERVICE_H_INLCUDE__
#define __NGSERVER_SERVICE_H_INLCUDE__

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Message.h"
#include "MessageQueue.h"
#include "Locker.h"
#include <chrono>

class Service : public boost::enable_shared_from_this < Service > 
{
public:
    // 单条消息最大缓冲
    static const int32_t kBufferSize = 65536;

    Service(int32_t sid) :
        _sid(sid){}

    
    inline int32_t GetSid() const 
    {
        return _sid;
    }

    inline void SetSid(int32_t sid)
    {
        _sid = sid;
    }
public:

    // 接口 初始化Service
    virtual bool Init();

    // 向Service推送消息
    bool PushMsg(Message* msg);

    // 处理消息队列中的消息 返回值代表消息队列是否还有新的消息
    bool Receive();

    // 接口 处理各类消息 返回true代表消息将由框架删除 返回false自行管理该消息
    virtual bool ProcessMsg(Message* msg);
    virtual bool ProcessMsg(TimerMessage* msg);
    virtual bool ProcessMsg(UserMessage* msg);
    virtual void ProcessMsg(CycleMessage* msg);
    virtual bool ProcessMsg(InsideMessage* msg);

public:
    static bool Send(int32_t sid, Message* msg);

    bool SendMsg(int32_t sid, int64_t sessionid, int16_t msgid)
    {
        InsideMessage* msg = new InsideMessage();
        msg->_dessid = sid;
        msg->_srcsid = GetSid();
        msg->_sessionid = sessionid;
        msg->_msgid = msgid;
        Service::Send(sid, msg);
    }

    template < typename MsgT > 
    bool SendMsg(int32_t sid, int64_t sessionid, int16_t msgid, MsgT& t)
    {
        InsideMessageT* msg = new InsideMessageT<MsgT>();
        msg->_dessid = sid;
        msg->_srcsid = GetSid();
        msg->_sessionid = sessionid;
        msg->_msgid = msgid;
        msg->_data = t;
        Service::Send(sid, msg);
    }
private:
    // 处理单条消息 返回值代表该消息是否需要被删除 
    bool ReceiveMsg(Message* msg);

private:
    MessageQueue<Message> _msgqueue;
    // 标识该Service的状态 lock表示当前有未处理的消息 通过_ready_lock.TryLock()来确认当前Service状态变化
    Locker _readylock;
    // 调试使用  用于确保同一时间只有一个线程在处理该Service的消息(通过 Receive 函数)
    Locker _recvcheck;

protected:
    int32_t _sid;


public: // 时间相关
    typedef std::chrono::steady_clock clock;
    static int64_t SteadyNow()
    {
        return clock::now().time_since_epoch().count();
    }
};


typedef std::shared_ptr<Service> ServicePtr;

#endif