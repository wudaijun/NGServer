#ifndef __NGSERVER_LOGINSERVICE_H_INCLUDE__
#define __NGSERVER_LOGINSERVICE_H_INCLUDE__

#include "MapService.h"
#include "LogService.h"
#include "DBService.h"
#include <vector>
#include <unordered_map>
#include "GameService.h"
#include "gamebasic/ServiceManager.h"
/*
*   ��¼���� LoginService
*   ����������ҵĵ�¼�ͳ�ʼ�����
*/
class LoginService : public GameService
{
public:
    LoginService(int32_t sid) :
        GameService(sid){}

    // �����Ϣע��
    bool Init() override;

    void StartMapServer(boost::asio::io_service* io);

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

    static uint16_t sDefaultSid;

#pragma region ע��ص�
    template<typename MsgEnum, typename F>
    void RegistSession(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_agent.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistSession(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = CallBackType::cbSessioDelegate;
        _session_agent.Regist((uint16_t)msgid, f, obj);
    }
#pragma endregion

#pragma region ������Ϣ

    bool ProcessMsg(UserMessage* msg) override;

#pragma endregion

#pragma region �ص�����
public:
    void OnLogin(PlayerSession* player, C2S_Login& msg);
#pragma endregion

private:
    // ��LoginService���������MapService
    std::vector<MapServicePtr> _mapservices;
    // ���ڵ�¼�е��û�
    std::unordered_map<int32_t, PlayerSessionPtr> _logging;

    DelegateManager<std::pair<PlayerSession*, ProtocolReader&>> _session_agent;
};

#endif