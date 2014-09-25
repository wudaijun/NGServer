#ifndef __NGSERVER_MESSAGE_H_INCLUDE__
#define __NGSERVER_MESSAGE_H_INCLUDE__
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