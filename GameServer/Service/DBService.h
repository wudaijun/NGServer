#ifndef __NGSERVER_DBSERVICE_H_INCLUDE__
#define __NGSERVER_DBSERVICE_H_INCLUDE__

class DBService : public Service
{
public:
    DBService(int32_t sid) :
        Service(sid){}
};

typedef std::shared_ptr<DBService> DBServicePtr;
#endif