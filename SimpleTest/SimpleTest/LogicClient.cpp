#include "LogicClient.h"
#include "RobotsManager.h"

LogicClient::LogicClient(IOService* io, int32_t id)
    :TcpClient(io, id), _time_stater(id)
{
    Init();
}

// ÏûÏ¢×¢²á
bool LogicClient::Init()
{
    _time_stater.SetMsgStatWay(C2S_Login::msgid, false);
    RegistMsg(S2C_Login::msgid, &LogicClient::OnLogin, this);
    RegistMsg(S2C_LoadCompleted::msgid, &LogicClient::OnLoadCompleted, this);
    return true;
}

void LogicClient::Login()
{
    if (!ConnectToServer())
        return;

    StartTime(C2S_Login::msgid);

    C2S_Login msg;
    msg.owner = "robot";
    msg.pwd = "iamrobot";
    SendMsg(msg);
} 

void LogicClient::OnLogin(S2C_Login& msg)
{
    EndTime(C2S_Login::msgid);

    std::cout << "++Robot[" << _id << "] Login Successed." << std::endl;
}

void LogicClient::OnLoadCompleted(S2C_LoadCompleted& msg)
{
    std::cout << "  Robot[" << _id << "] Load Completed." << std::endl;
    std::cout << "  Server: " << msg.teststr << std::endl;
}
