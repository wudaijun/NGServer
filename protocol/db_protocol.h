#ifndef __NGSERVER_DBPROTOCOL_H_INCLUDE__
#define __NGSERVER_DBPROTOCOL_H_INCLUDE__

#include "protocol.h"

/*
*   DB相关协议 以及服务内部通信协议
*   由于GameService和DBService的消息是线程间消息，因此对于非POD类型无需特例化
*   甚至可以直接定义struct 内含消息ID 即可
*/

namespace NGServer{
namespace protocol{

/****************************/
/*       DB协议消息ID       */
/***************************/
enum DBMsgId
{
    kS2D_Start = 10000,
    kS2D_PlayerLogin,           // 玩家登录
    kD2S_PlayerLoginResult,     // 登录结果
    kS2D_LoadPlayer,            // 加载玩家数据
    kD2S_PlayerLoadCompleted,   // 玩家数据加载完成

};

// 绑定消息ID 宏
#define DBMSGID(id) static const DBMsgId msgid = (NGServer::protocol::DBMsgId::id);

/****************************/
/*  服务内部通信协议消息ID   */
/***************************/
enum SSMsgId
{
    kSS_Start = 20000,
    kSS_PlayerLogin,    // 玩家登录
};

// 绑定消息ID 宏
#define SSMSGID(id) static const SSMsgId msgid = (NGServer::protocol::SSMsgId::id);

/**************************/
/*        协议内容        */
/*************************/
struct S2D_PlayerLogin
{
    DBMSGID(kS2D_PlayerLogin);
    std::string owner;
    std::string pwd;
    uint32_t connid;
};


struct D2S_PlayerLoginResult
{
    DBMSGID(kD2S_PlayerLoginResult);
    std::string owner;
    std::string pwd;
    uint32_t connid;
    int64_t playerid; // 玩家唯一标识
    // 一些其他数据
};

struct S2D_LoadPlayer
{
    DBMSGID(kS2D_LoadPlayer);
    int64_t playerid;   // 加载的玩家ID
    // ....
};

struct D2S_PlayerLoadCompleted
{
    DBMSGID(kD2S_PlayerLoadCompleted);
    int64_t playerid;   // 完成加载的玩家ID
    // ....
};

}
}
#endif