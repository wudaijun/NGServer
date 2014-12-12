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

    bool IsAllOffline();

    int32_t GetCurOnline();

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

    static uint16_t sDefaultSid;


#pragma region �ص�����
public:
    void OnPlayerLogin(PlayerSession* session, C2S_Login& msg);
    void OnDBPlayerLogin(PlayerSession* session, D2S_PlayerLoginResult& msg);
    void OnPlayerLogout(PlayerSession* session) override;
#pragma endregion

private:
    // ��LoginService���������MapService
    std::vector<MapServicePtr> _mapservices;
    // ���ڵ�¼�е��û� key : �û���  value : �û�����ID
    std::unordered_map<std::string, uint32_t> _logging;
};

#endif