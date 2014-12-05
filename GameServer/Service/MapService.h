#ifndef __NGSERVER_MAPSERVICE_H_INLCUDE__
#define __NGSERVER_MAPSERVICE_H_INLCUDE__

#include "GameService.h"

class MapService : public GameService
{
public:
    MapService(int32_t sid) :
        GameService(sid){}

    bool ProcessMsg(UserMessage* msg) override;

    bool Send(const char* data, size_t len) override
    {
        return true;
    }

#pragma region 注册回调
    // 注册第一参数为Player*的回调函数
    template<typename MsgEnum, typename F>
    void RegistPlayer(MsgEnum msgid, F f)
    {
        _calltype[(uint16_t)msgid] = cbPlayerDelegate;
        _player_delegate.Regist((uint16_t)msgid, f);
    }

    template<typename MsgEnum, typename F, typename ObjT>
    void RegistPlayer(MsgEnum msgid, F f, ObjT* obj)
    {
        _calltype[(uint16_t)msgid] = cbPlayerDelegate;
        _player_delegate.Regist((uint16_t)msgid, f, obj);
    }
#pragma endregion

private:
    DelegateManager<std::pair<Player*, ProtocolReader&>> _player_delegate;
};

typedef std::shared_ptr<MapService> MapServicePtr;
#endif