#ifndef __NGSERVER_GAMESERVICE_H_INCLUDE__
#define __NGSERVER_GAMESERVICE_H_INCLUDE__

#include "protocol/protocol.h"
#include "gamebasic/Service.h"
#include "common/AutoCall.h"
#include "gamebasic/AutoCallSpecial.h"

using namespace NGServer::protocol;
using namespace NGServer::common;

/*
*   ��Ϸ������ࣺ
*       1. �����־����ӿں����ݿ�ӿ� һ��GameService�ɰ�һ��DBService��һ��LogService
*       2. �����Ϣע�����
*       3. ���һЩȺ���ȹ���
*/

class Player;
class PlayerSession;
class SessionManager;

class GameService : public Service
{

#pragma region ���ù�������
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

    // ��ProcessMsg��Ϣ�ص���ʹ�� ��ʹ��cbPlayerInsideDelegate��Service��д
    virtual std::shared_ptr<Player> GetPlayer(int64_t playerid)
    {
        return nullptr;
    }

    virtual void OnPlayerLogout(PlayerSession* session);

#pragma endregion

#pragma region ������Ϣ
public:

    bool ProcessMsg(InsideMessage* msg) override;
    bool ProcessMsg(UserMessage* msg) override;
    
#pragma endregion

#pragma region ������Ϣ
public:


    // ���͵����ݿ�
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

    // ���͵���־
    template<typename T>
    bool SendToLog(uint16_t msgid, T& t)
    {
        return Service::SendMsg(_logservice_sid, 0, msgid, t);
    }

    // ���͵��ͻ���
    template<typename T>
    bool SendToClient(uint16_t msgid, T& t)
    {
        char buf[kBufferSize];
        ProtocolWriter encoder(buf, kBufferSize);

#ifdef _DEBUG // �Ա�������һЩ���
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

    // �ӿ� ���彫���ݷ��͸��ͻ��˵Ľӿ� ������Ⱥ�� or ����
    virtual bool Send(const char* data, size_t len) = 0;

#pragma endregion

#pragma region ��Ϣע��ͻص�����
public:
    enum CallBackType
    {
        cbPlayerDelegate = 1,   // �ص�������ʽ�� ��һ������: Player*           �ڶ�����������ProtocolReader�ж�ȡ
        cbSessioDelegate,       // �ص�������ʽ�� ��һ������: PlayerSession*    �ڶ�����������ProtocolReader�ж�ȡ
        cbInsideDelegate,       // �ص�������ʽ�� ��һ������: ��InsideMessage�ж�ȡ
        cbSessionInsideDelegate,// �ص�������ʽ�� ��һ������: PlayerSession*    �ڶ�����������InsideMessage�ж�ȡ
        cbPlayerInsideDelegate, // �ص�������ʽ�� ��һ������: Player*           �ڶ�����������InsideMessage�ж�ȡ
    };
    
    // �ص����ͣ�cbPlayerDelegate
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
    

    // �ص����ͣ� cbSessionDelegate
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
    // �ص����ͣ� cbInsideDelegate
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

    // �ص����ͣ� cbSessionInsideDelegate
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

    // �ص����ͣ� cbPlayerInsideDelegate
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

    // ����ע�� �ص���������һ������ΪPlayer* �ڶ���������ProtocolReader�ж�ȡ
    DelegateManager<std::pair<Player*, ProtocolReader&>> _player_delegate;
    // ����ע�� �ص���������һ������ΪPlayerSession* �ڶ���������ProtocolReader�ж�ȡ
    DelegateManager<std::pair<PlayerSession*, ProtocolReader&>> _session_delegate;
    // ����ע�� �ص�������ֻ��һ������  ��InsideMessage�ж�ȡ
    DelegateManager<InsideMessage> _inside_delegate;
    // ����ע�� �ص���������һ������ΪPlayerSession* �ڶ���������InsideMessage�ж�ȡ
    DelegateManager<std::pair<PlayerSession*, InsideMessage*>> _session_inside_delegate;
    // ����ע�� �ص���������һ������ΪPlayer* �ڶ���������InsideMessage�ж�ȡ
    DelegateManager<std::pair<Player*, InsideMessage*>> _player_inside_delegate;
};

#endif
