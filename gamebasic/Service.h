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
    // ������Ϣ��󻺳�
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

    // �ӿ� ��ʼ��Service
    virtual bool Init();

    // ��Service������Ϣ
    bool PushMsg(Message* msg);

    // ������Ϣ�����е���Ϣ ����ֵ������Ϣ�����Ƿ����µ���Ϣ
    bool Receive();

    // �ӿ� ���������Ϣ ����true������Ϣ���ɿ��ɾ�� ����false���й������Ϣ
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
    // ��������Ϣ ����ֵ�������Ϣ�Ƿ���Ҫ��ɾ�� 
    bool ReceiveMsg(Message* msg);

private:
    MessageQueue<Message> _msgqueue;
    // ��ʶ��Service��״̬ lock��ʾ��ǰ��δ�������Ϣ ͨ��_ready_lock.TryLock()��ȷ�ϵ�ǰService״̬�仯
    Locker _readylock;
    // ����ʹ��  ����ȷ��ͬһʱ��ֻ��һ���߳��ڴ����Service����Ϣ(ͨ�� Receive ����)
    Locker _recvcheck;

protected:
    int32_t _sid;


public: // ʱ�����
    typedef std::chrono::steady_clock clock;
    static int64_t SteadyNow()
    {
        return clock::now().time_since_epoch().count();
    }
};


typedef std::shared_ptr<Service> ServicePtr;

#endif