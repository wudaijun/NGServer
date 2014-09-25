#ifndef __NGSERVER_MESSAGE_H_INCLUDE__
#define __NGSERVER_MESSAGE_H_INCLUDE__
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
class TimerMessage
{
public:
    TimerMessage(void* data = nullptr)
        :_data(data), _cancel(false){}
    MessageType GetType() const 
    { 
        return MessageType::kTimerMessage;
    }

private:
    void* _data;
    bool _cancel;
};

#endif