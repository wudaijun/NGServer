#ifndef __NGSERVER_LOGSERVICE_H_INCLUDE__
#define __NGSERVER_LOGSERVICE_H_INCLUDE__

#include "GameService.h"

class LogService : public Service
{
    LogService(int32_t sid) :
        Service(sid){}
};

typedef std::shared_ptr<LogService> LogServicePtr;

#endif