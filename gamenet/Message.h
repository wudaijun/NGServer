#ifndef __NGSERVER_MESSAGE_H_INCLUDE__
#define __NGSERVER_MESSAGE_H_INCLUDE__
#include <stdint.h>
#include "Locker.h"
#include <boost/weak_ptr.hpp>

/*
*       File: Message.h �������Message��Ϣ
*    Created: 2014-09-25 �賿. By wudaijun
*/

// ��Ϣ����
enum MessageType
{
    kUserMessage = 1,   // �û���Ϣ
    kTimerMessage,      // ��ʱ����Ϣ
    kCycleMessage,      // ֡��Ϣ (����ÿһ��һ��)
    kInsideMessage,     // �ڲ���Ϣ(����/�����ڲ�)
};

// ������Ϣ�Ļ��� �ṩ���������ʶ��
class Message
{
public:
    virtual MessageType GetType() const = 0;
    virtual ~Message(){}
};

// ��ʱ����Ϣ
class TimerMessage : public Message
{
public:
    TimerMessage(void* data = nullptr)
        :_data(data), _cancel(false){}

    MessageType GetType()  const override
    { 
        return MessageType::kTimerMessage;
    }

    
public:// �ص����� ����ֵ>0�����´���Ҫִ�е�ʱ��
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

// �ͻ��˵���Ϣ
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

// ����֮��ͨ�ŵ���Ϣ
class InsideMessage : public Message
{
public:
    MessageType GetType()  const override
    { 
        return MessageType::kInsideMessage; 
    }

public:
    int32_t _srcsid;    // Դ����ID
    int32_t _dessid;    // Ŀ�����ID
    int64_t _sessionid; // �Ự������ID
    int32_t _msgid;     // ��ϢID
};

// �����ݵ�InsideMessage
template< typename T >
class InsideMessageT : public InsideMessage
{
public:
    T _data;
};

// ѭ��ִ�е���Ϣ
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

    // �õ�ִ�е�ʱ����
    inline int32_t GetProid() const
    {
        return _priod;
    }

    // �����´�ִ�е�ʱ��
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

    // ȡ��ѭ����ʱ��
    inline void Cancel()
    {
        _priod = -1;
    }

public:
    int32_t _priod;                     // ѭ��ִ�м��
    int64_t _nexttime;                  // �´�ִ��ʱ��
    std::weak_ptr<Service> _service;    // ����Service
    Locker _locker;                     
};

#endif