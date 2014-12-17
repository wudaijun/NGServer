#ifndef __SIMPLE_TEST_LOGICCLIENT_H_INCLUDE__
#define __SIMPLE_TEST_LOGICCLIENT_H_INCLUDE__

#include "TcpClient.h"
#include "TimeStater.h"

class RobotsManager;

class LogicClient : public TcpClient
{

public:
    LogicClient(IOService* io, int32_t id=0);

    // ��Ϣע��
    bool Init() override;

    void Connected(const boost::system::error_code& err) override;

// ͳ��ʱ��
public:
    inline void StartTime(uint16_t msgid)
    {
        _time_stater.TimeStart(msgid);
    }

    inline bool EndTime(uint16_t msgid, int32_t critical_ms=20)
    {
        return _time_stater.TimeEnd(msgid, critical_ms);
    }

// ���͸��ֲ�����Ϣ
public:
    void Login();

// ��Ӧ����������Ϣ
public:
    void OnLogin(S2C_Login& msg);
    void OnLoadCompleted(S2C_LoadCompleted& msg);

private:
    TimeStater _time_stater;

};

typedef std::shared_ptr<LogicClient> LogicClientPtr;

#endif