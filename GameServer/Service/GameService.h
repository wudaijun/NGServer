#ifndef __NGSERVER_GAMESERVICE_H_INCLUDE__
#define __NGSERVER_GAMESERVICE_H_INCLUDE__

#include "gamebasic/Service.h"
#include "common/AutoCall.h"
#include "gamebasic/AutoCallSpecial.h"
#include "protocol/protocol.h"
#include "../Player/SessionManager.h"

using namespace NGServer::protocol;
using namespace NGServer::common;

/*
*   ��Ϸ������ࣺ
*       1. �����־����ӿں����ݿ�ӿ� һ��GameService�ɰ�һ��DBService��һ��LogService
*       2. �����Ϣע�����
*       3. ���һЩȺ���ȹ���
*/

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

    void SetSessionManager(SessionManager* manager)
    {
        _session_manager = manager;
    }
#pragma endregion

#pragma region ������Ϣ
public:

    bool ProcessMsg(InsideMessage* msg) override;
    
#pragma endregion

#pragma region ������Ϣ
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

    // ���͵����ݿ�
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

    // ���͵���־
    template<typename T>
    bool SendToLog(uint16_t msgid, const T& t)
    {
        return SendInsideMsg(_dbservice_sid, 0, msgid, t);
    }

    // ���͵��ͻ���
    template<typename T>
    bool SendToClient(uint16_t msgid, const T& t)
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
        cbPlayerDelegate = 1,  // ��Player*Ϊ��һ�������Ļص�����
        cbPlayerMethod   = 2, // �ص�Player�ķ���
        cbSessioDelegate = 3, // �ص���PlayerSession*Ϊ��һ�������Ļص�����
    };
    /*
    template<typename R, typename ObjT, typename T1, typename T2>
    void RegistPlayer(R(ObjT::*f)(T1, T2), ObjT* obj)
    {
        uint16_t msgid = _player_delegate.RegistMsg(f, obj);
        _calltype[msgid] = cbPlayerDelegate;
    }

    template < typename R, typename T1, typename T2 > 
    void RegistPlayer(R(*f)(T1 ,T2))
    {
        uint16_t msgid = _player_delegate.RegistMsg(f);
        _calltype[msgid] = cbPlayerDelegate;
    }
    */


#pragma endregion


protected:
    int32_t _logservice_sid;
    int32_t _dbservice_sid;

    CallBackType _calltype[256 * 256];

    SessionManager* _session_manager;
};

#endif