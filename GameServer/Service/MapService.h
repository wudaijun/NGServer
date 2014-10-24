#ifndef __NGSERVER_MAPSERVICE_H_INCLUDE__
#define __NGSERVER_MAPSERVICE_H_INLCUDE__

#include "GameService.h"

class MapService : public GameService
{
public:
    MapService(int32_t sid) :
        GameService(sid){}

    bool Send(const char* data, size_t len) override
    {
        return true;
    }
};

typedef std::shared_ptr<MapService> MapServicePtr;
#endif