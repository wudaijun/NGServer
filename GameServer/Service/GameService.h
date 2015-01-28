#ifndef __NGSERVER_GAMESERVICE_H_INCLUDE__
#define __NGSERVER_GAMESERVICE_H_INCLUDE__

#include "protocol/protocol.h"
#include "gamebasic/Service.h"
#include "common/AutoCall.h"
#include "gamebasic/AutoCallSpecial.h"

using namespace NGServer::protocol;
using namespace NGServer::common;

/*
*   游戏服务基类：
*       1. 添加日志服务接口和数据库接口 一个GameService可绑定一个DBService和一个LogService
*       2. 添加消息注册机制
*       3. 添加一些群发等功能
*/

class Player;
class PlayerSession;
class SessionManager;

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

    void SetSessionManager(std::shared_ptr<SessionManager>& manager)
    {
        _session_manager = manager;
    }

    // 在ProcessMsg消息回调中使用 由使用cbPlayerInsideDelegate的Service改写
    virtual std::shared_ptr<Player> GetPlayer(int64_t playerid)
    {
        return nullptr;
    }

    virtual void OnPlayerLogout(PlayerSession* session);

#pragma endregion

#pragma region 处理消息
public:

    bool ProcessMsg(InsideMessage* msg) override;
    bool ProcessMsg(UserMessage* msg) override;
    
#pragma endregion

#pragma region 发送消息
public:


    // 发送到数据库
    template<typename T>
    bool SendToDB(int64_t sessionid, DBMsgId msgid, T& t)
    {
        return Service::SendMsg(_dbservice_sid, sessionid, (uint16_t)msgid, t);
    }

    template<typename T>
    bool SendToDB(DBMsgId msgid, T& t)
    {
        return Service::SendMsg(_dbservice_sid, 0, (uint16_t)msgid, t);
    }

    template<typename T>
    bool SendToDB(T& t)
    {
        return Service::SendMsg(_dbservice_sid, 0, (uint16_t)T::msgid, t);
    }

    template<typename T>
    bool SendToDB(int64_t sessionid, DBMsgId msgid)
    {
        return Service::SendMsg(_dbservice_sid, sessionid, (uint16_t)msgid);
    }

    template<typename T>
    bool SendToDB(DBMsgId msgid)
    {
        return Service::SendMsg(_dbservice_sid, 0, (uint16_t)msgid);
    }

    // 发送到日志
    template<typename T>
    bool SendToLog(uint16_t msgid, T& t)
    {
        return Service::SendMsg(_logservice_sid, 0, msgid, t);
    }

    // 发送到客户端
    template<typename T>
    bool SendToClient(uint16_t msgid, T& t)
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
        cbPlayerDelegate = 1,   // 回调函数形式： 第一个参数: Player*           第二个参数：从ProtocolReader中读取
        cbSessioDelegate,       // 回调函数形式： 第一个参数: PlayerSession*    第二个参数：从ProtocolReader中读取
        cbInsideDelegate,       // 回调函数形式： 第一个参数: 从InsideMessage中读取
        cbSessionInsideDelegate,// 回调函数形式： 第一个参数: PlayerSession*    第二个参数：从InsideMessage中读取
        cbPlayerInsideDelegate, // 回调函数形式： 第一个参数: Player*           第二个参数：从InsideMessage中读取
    };
    
    // 回调类型：cbPlayerDelegate
    template<typename MsgEnum, typename F, typename ObjT>
    void RegistPlayer(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[msgid] = cbPlayerDelegate;
        _player_delegate.Regist(msgid, f, obj);
    }

    template < typename R, typename T1, typename T2 > 
    void RegistPlayer(R(*f)(T1 ,T2))
    {
		uint16_t msgid = T1::msgid;
        _calltype[msgid] = cbPlayerDelegate;
        _player_delegate.Regist(msgid, f);
    }
    

    // 回调类型： cbSessionDelegate
    template<typename MsgEnum, typename F>
    void RegistSession(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_delegate.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistSession(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_delegate.Regist((uint16_t)msgid, f, obj);
    }
    // 回调类型： cbInsideDelegate
    template<typename MsgEnum, typename F>
    void RegistInside(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbInsideDelegate;
        _inside_delegate.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistInside(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbInsideDelegate;
        _inside_delegate.Regist((uint16_t)msgid, f, obj);
    }

    // 回调类型： cbSessionInsideDelegate
    template<typename MsgEnum, typename F>
    void RegistSessionInside(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessionInsideDelegate;
        _session_inside_delegate.Regist((uint16_t)msgid, f);
    }
    template<typename MsgEnum, typename F, typename ObjT>
    void RegistSessionInside(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessionInsideDelegate;
        _session_inside_delegate.Regist((uint16_t)msgid, f, obj);
    }

    // 回调类型： cbPlayerInsideDelegate
    template<typename MsgEnum, typename F>
    void RegistPlayerInside(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbPlayerInsideDelegate;
        _player_inside_delegate.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistPlayerInside(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbPlayerInsideDelegate;
        _player_inside_delegate.Regist((uint16_t)msgid, f, obj);
    }
#pragma endregion


protected:
    int32_t _logservice_sid;
    int32_t _dbservice_sid;

    CallBackType _calltype[256 * 256];

    std::shared_ptr<SessionManager> _session_manager;

private:

    // 管理注册 回调函数：第一个参数为Player* 第二个参数从ProtocolReader中读取
    DelegateManager<std::pair<Player*, ProtocolReader&>> _player_delegate;
    // 管理注册 回调函数：第一个参数为PlayerSession* 第二个参数从ProtocolReader中读取
    DelegateManager<std::pair<PlayerSession*, ProtocolReader&>> _session_delegate;
    // 管理注册 回调函数：只含一个参数  从InsideMessage中读取
    DelegateManager<InsideMessage> _inside_delegate;
    // 管理注册 回调函数：第一个参数为PlayerSession* 第二个参数从InsideMessage中读取
    DelegateManager<std::pair<PlayerSession*, InsideMessage*>> _session_inside_delegate;
    // 管理注册 回调函数：第一个参数为Player* 第二个参数从InsideMessage中读取
    DelegateManager<std::pair<Player*, InsideMessage*>> _player_inside_delegate;
};

#endif
