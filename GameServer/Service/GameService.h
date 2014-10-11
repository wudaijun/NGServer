#ifndef __NGSERVER_GAMESERVICE_H_INCLUDE__
#define __NGSERVER_GAMESERVICE_H_INCLUDE__

#include "../../gamebasic/Service.h"

/*
*   游戏服务基类：
*       1. 添加日志服务接口和数据库接口 一个GameService可绑定一个DBService和一个LogService
*       2. 添加消息注册机制
*       3. 添加一些群发等功能
*/

class GameService : public Service
{

};

#endif