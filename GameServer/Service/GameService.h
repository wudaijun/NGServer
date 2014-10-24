#ifndef __NGSERVER_GAMESERVICE_H_INCLUDE__
#define __NGSERVER_GAMESERVICE_H_INCLUDE__

#include "../../gamebasic/Service.h"
#include "../../common/AgentCall.h"
#include "../../protocol/protocol.h"
#include "../Player/PlayerManager.h"

using namespace NGServer::protocol;
using namespace NGServer::common;

/*
*   游戏服务基类：
*       1. 添加日志服务接口和数据库接口 一个GameService可绑定一个DBService和一个LogService
*       2. 添加消息注册机制
*       3. 添加一些群发等功能
*/

class GameService : public Service
{

#pragma region 设置关联服务
public:
    GameService(int32_t sid) :
        Service(sid){}

    void SetLogService(int32_t logservice_sid)
    {
        _logservice_sid = logservice_sid;
    }

    void SetDBService(int32_t dbservice_sid)
    {
        _dbservice_sid = dbservice_sid;
    }

    void SetPlayerManager(PlayerManager* manager)
    {
        _player_manager = manager;
    }
#pragma endregion

#pragma region 处理消息
public:
    bool ProcessMsg(UserMessage* msg) override;
    bool ProcessMsg(InsideMessage* msg) override;
    
#pragma endregion

#pragma region 发送消息
public:
    template<typename T>
    bool SendInsideMsg(int32_t sid, int64_t sessionid, uint16_t msgid, const T& t)
    {
        InsideMessageT<T>* insidemsg = new InsideMessageT<T>();
        insidemsg->_dessid = sid;
        insidemsg->_srcsid = _sid;
        insidemsg->_sessionid = sessionid;
        insidemsg->_msgid = msgid;
        insidemsg->_data = t;
        return Service::Send(sid, insidemsg);
    }

    bool SendInsideMsg(int32_t sid, int64_t sessionid, uint16_t msgid)
    {
        InsideMessage* insidemsg = new InsideMessage();
        insidemsg->_dessid = sid;
        insidemsg->_srcsid = _sid;
        insidemsg->_sessionid = sessionid;
        insidemsg->_msgid = msgid;
        return Service::Send(sid, insidemsg);
    }

    // 发送到数据库
    template<typename T>
    bool SendToDB(int64_t sessionid, uint16_t msgid, const T& t)
    {
        return SendInsideMsg<T>(_dbservice_sid, sessionid, msgid, t);
    }

    template<typename T>
    bool SendToDB(uint16_t msgid, const T& t)
    {
        return SendInsideMsg<T>(_dbservice_sid, 0, msgid, t);
    }

    template<typename T>
    bool SendToDB(int64_t sessionid, uint16_t msgid)
    {
        return SendInsideMsg(_dbservice_sid, sessionid, msgid);
    }

    template<typename T>
    bool SendToDB(uint16_t msgid)
    {
        return SendInsideMsg(_dbservice_sid, 0, msgid);
    }

    // 发送到日志
    template<typename T>
    bool SendToLog(uint16_t msgid, const T& t)
    {
        return SendInsideMsg(_dbservice_sid, 0, msgid, t);
    }

    // 发送到客户端
    template<typename T>
    bool SendToClient(uint16_t msgid, const T& t)
    {
        char buf[kBufferSize];
        ProtocolWriter encoder(buf, kBufferSize);

#ifdef _DEBUG // 对编解码进行一些检查
        int32_t msglen = ProtocolWriter::kHeadLen + ProtocolWriter::kMsgIdLen;
        msglen += GetMsgSize(t);

        if (msglen < kBufferSize)
        {
            if (AutoEncode(encoder, t))
            {
                int32_t len = encoder.WriteHead(msgid);
                assert(len == msglen);
                return Send(buf, len);
            }
        }
#else
        if(AutoEncode(encoder, t))
        {
            int32_t len = encoder.WriteHead(msgid);
            return Send(buf, len);
        }
#endif
        std::cerr << " ##### msg encoder error. msgid: " << msgid << "######" << std::endl;
        return false;
    }

    // 接口 定义将数据发送给客户端的接口 可以是群发 or 单发
    virtual bool Send(const char* data, size_t len) = 0;

#pragma endregion

#pragma region 消息注册和回调机制
public:
    enum CallBackType
    {
        cbPlayerAgent = 0,  // 以Player为第一个参数的回调函数
    };
    template<typename MsgEnum, typename F>
    void RegistPlayer(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = cbPlayerAgent;
        _player_agent.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistPlayer(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = cbPlayerAgent;
        _player_agent.Regist((uint16_t)msgid, f, obj);
    }

    template<typename R, typename ObjT, typename T1, typename T2>
    void RegistPlayer(R(ObjT::*f)(T1, T2), ObjT* obj)
    {
        uint16_t msgid = _player_agent.RegistMsg(f, obj);
        _calltype[msgid] = cbPlayerAgent;
    }

    template < typename R, typename T1, typename T2 > 
    void RegistPlayer(R(*f)(T1 ,T2))
    {
        uint16_t msgid = _player_agent.RegistMsg(f);
        _calltype[msgid] = cbPlayerAgent;
    }


#pragma endregion


protected:
    int32_t _logservice_sid;
    int32_t _dbservice_sid;

    CallBackType _calltype[256 * 256];
    AgentManager<Player&, ProtocolReader> _player_agent;
    PlayerManager* _player_manager;
};

#endif