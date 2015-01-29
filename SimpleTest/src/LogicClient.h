#ifndef __SIMPLE_TEST_LOGICCLIENT_H_INCLUDE__
#define __SIMPLE_TEST_LOGICCLIENT_H_INCLUDE__

#include "TcpClient.h"
#include "TimeStater.h"

class RobotsManager;

class LogicClient : public TcpClient
{

public:
    LogicClient(IOService* io, int32_t id=0);

    // 消息注册
    bool Init() override;

    void Connected(const boost::system::error_code& err) override;

// 统计时间
public:
    inline void StartTime(uint16_t msgid)
    {
        _time_stater.TimeStart(msgid);
    }

    inline bool EndTime(uint16_t msgid, int32_t critical_ms=20)
    {
        return _time_stater.TimeEnd(msgid, critical_ms);
    }

// 发送各种测试消息
public:
    void Login();

// 响应服务器的消息
public:
    void OnLogin(S2C_Login& msg);
    void OnLoadCompleted(S2C_LoadCompleted& msg);

private:
    TimeStater _time_stater;

};

typedef std::shared_ptr<LogicClient> LogicClientPtr;

#endif