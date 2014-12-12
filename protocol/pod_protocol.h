#ifndef __NGSERVER_POD_PROTOCOL_H_INCLUDE__
#define __NGSERVER_POD_PROTOCOL_H_INCLUDE__

#include "msgid.h"

namespace NGServer
{
namespace protocol
{

#define NETMSG(T) \
struct T \


NETMSG(S2C_Login)
{
    NETMSGID(kS2C_Login);
    bool login_success;     // ÊÇ·ñµÇÂ¼³É¹¦
};


}
}
#endif