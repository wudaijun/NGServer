#ifndef __NGSERVER_MESSAGE_H_INCLUDE__
#define __NGSERVER_MESSAGE_H_INCLUDE__
#include <stdint.h>
#include "Locker.h"
#include <boost/weak_ptr.hpp>

/*
*       File: Message.h 定义各种Message消息
*    Created: 2014-09-25 凌晨. By wudaijun
*/

// 消息类型
enum MessageType
{
    kUserMessage = 1,   // 用户消息
    kTimerMessage,      // 定时器消息
    kCycleMessage,      // 帧消息 (比如每一秒一次)
    kInsideMessage,     // 内部消息(服务/进程内部)
};

// 所有消息的基类 提供本身的类型识别
class Message
{
public:
    virtual MessageType GetType() const = 0;
    virtual ~Message(){}
};

// 定时器消息
class TimerMessage : public Message
{
public:
    TimerMessage(void* data = nullptr)
        :_data(data), _cancel(false){}

    MessageType GetType()  const override
    { 
        return MessageType::kTimerMessage;
    }

    
public:// 回调函数 返回值>0代表下次需要执行的时间
    std::function<int32_t()> _func;

public:
    void Cancel()
    {
        _cancel = true;
    }

    bool IsCancel() const
    {
        return _cancel;
    }

    inline void* GetData() const 
    {
        return _data;
    }

private:
    void* _data;
    bool _cancel;
};

// 客户端的消息
class UserMessage : public Message
{
public:
    UserMessage(const char* data, size_t len)
    {
        if (data != nullptr)
        {
            _data = new char[len];
            memcpy(_data, data, len);
        }
        else
            _data = nullptr;
    }

    MessageType GetType()  const override
    {
        return MessageType::kUserMessage;
    }

public:
    char* _data;
    size_t _len;
};

// 
template< typename T >
class UserMessageT : public UserMessage
{
public:
    UserMessageT(const char* data, size_t len, T user) :
        UserMessage(data, len), _user(user){}

    inline T GetClient() const { return _user;  }
public:
    T _user;
};

// 服务之间通信的消息
class InsideMessage : public Message
{
public:
    MessageType GetType()  const override
    { 
        return MessageType::kInsideMessage; 
    }

public:
    int32_t _srcsid;    // 源服务ID
    int32_t _dessid;    // 目标服务ID
    int64_t _sessionid; // 会话或连接ID
    int32_t _msgid;     // 消息ID
};

// 带内容的InsideMessage
template< typename T >
class InsideMessageT : public InsideMessage
{
public:
    T _data;
};

// 循环执行的消息
class Service;
typedef std::shared_ptr<Service> ServicePtr;
class CycleMessage : public Message
{
public:
    CycleMessage(const ServicePtr& s, int64_t t, int32_t priod)
        : _service(s), _nexttime(t), _priod(priod){}

    MessageType GetType()  const override
    {
        return MessageType::kCycleMessage;
    }

    static CycleMessage* Create(ServicePtr sptr, int32_t proid);

    inline bool TryLock()
    {
        return _locker.TryLock();
    }

    inline void UnLock()
    {
        return _locker.UnLock();
    }

    // 得到执行的时间间隔
    inline int32_t GetProid() const
    {
        return _priod;
    }

    // 计算下次执行的时间
    bool ReadyNextTime()
    {
        if (_priod > 0)
        {
            _nexttime += _priod;
            return true;
        }
        else
            return false;
    }

    // 取消循环计时器
    inline void Cancel()
    {
        _priod = -1;
    }

public:
    int32_t _priod;                     // 循环执行间隔
    int64_t _nexttime;                  // 下次执行时间
    std::weak_ptr<Service> _service;    // 所属Service
    Locker _locker;                     
};

#endif