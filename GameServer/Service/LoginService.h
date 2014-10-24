#ifndef __NGSERVER_LOGINSERVICE_H_INCLUDE__
#define __NGSERVER_LOGINSERVICE_H_INCLUDE__

#include "MapService.h"
#include "LogService.h"
#include "DBService.h"
#include <vector>
#include "GameService.h"
#include "../../gamebasic/ServiceManager.h"
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

#pragma region �ص�����
public:
    void OnLogin(Player& player, C2S_Login& msg)
    {
        cout << "-- C2S_Login --" << endl;
        cout << "- name: " << msg.name<<endl;
        cout << "- pwd: " << msg.pwd << endl;
        cout << "---------------" << endl;
        player._name = msg.name;
        _player_manager->PrintPlayerName(player.GetConnId());
    }
#pragma endregion

private:
    std::vector<MapServicePtr> _mapservices;
    
};

#endif