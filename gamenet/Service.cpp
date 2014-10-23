#include "Service.h"
#include "ServiceManager.h"

bool Service::Init()
{
    return true;
}

bool Service::PushMsg(Message* msg)
{
    if (_msgqueue.PushMsg(msg) > 0)
    {
        return _readylock.TryLock();
    }
    return false;
}

// ȡ����Ϣ�����е���Ϣ  ����ReceiveMsg������Ϣ
// ���������֮�� �����л���ʣ����Ϣ �򷵻�true ���򷵻�false
bool Service::Receive()
{
#ifdef _DEBUG
    if (!_recvcheck.TryLock())
    {
        std::cerr << " # service Receive is not runing in single thread ! " << std::endl;
        assert(0);
    }
#endif
    std::vector<Message*>* msgs = _msgqueue.PopAll();
    size_t size = msgs->size();
    for (size_t i = 0; i < size; i++)
    {
        Message* msg = (*msgs)[i];
        std::unique_ptr<Message> autodel(msg);
        if (!ReceiveMsg(msg))
        {
            autodel.release();
        }
    }
    msgs->clear();

    if (_msgqueue.Size())
        return true;

#ifdef _DEBUG
    _recvcheck.UnLock();
#endif

    _readylock.UnLock();
    return false;
}

// ʶ����Ϣ���� ͨ��ProcessMsg���������Ϣ
bool Service::ReceiveMsg(Message* msg)
{
    MessageType type = msg->GetType();
    bool freemsg = true; // �Ƿ���Ҫɾ������Ϣ ������ֵ
    switch (type)
    {
        case kTimerMessage:
        {
            TimerMessage* timermsg = dynamic_cast<TimerMessage*>(msg);
            if (timermsg != nullptr)
            {
                if (!timermsg->IsCancel())
                {
                    int32_t nexttime = timermsg->_func();
                    if (nexttime > 0)
                    {

                    }
                }
            }
            break;
        }
        case kUserMessage:
        {
            UserMessage* usermsg = dynamic_cast<UserMessage*>(msg);
            if (usermsg != nullptr)
            {
                freemsg = ProcessMsg(usermsg);
            }
            break;
        }
        case kCycleMessage:
        {
            CycleMessage* cyclemsg = dynamic_cast<CycleMessage*>(msg);
            if (cyclemsg != nullptr)
            {
                ProcessMsg(cyclemsg);
            }
            freemsg = false;
            break;
        }
        case kInsideMessage:
        {
            InsideMessage* insidemsg = dynamic_cast<InsideMessage*>(msg);
            if (insidemsg != nullptr)
            {
                freemsg = ProcessMsg(insidemsg);
            }
            break;
        }
        default:
            freemsg = ProcessMsg(msg);
            break;
    }
    return freemsg;
}

bool Service::ProcessMsg(Message* msg)
{
    return true;
}

bool Service::ProcessMsg(UserMessage* msg)
{
    return true;
}

bool Service::ProcessMsg(TimerMessage* msg)
{
    return true;
}

void Service::ProcessMsg(CycleMessage* msg)
{
    
}

bool Service::ProcessMsg(InsideMessage* msg)
{
    return true;
}

bool Service::Send(int32_t sid, Message* msg)
{
    return ServiceManager::Send(sid, msg);
}   