#ifndef __NGSERVER_MSGID_H_INCLUDE__
#define __NGSERVER_MSGID_H_INCLUDE__

namespace NGServer
{
namespace protocol
{

#define NETMSGID(id) static const uint16_t msgid = static_cast<uint16_t>(MsgId::id); 

enum MsgId : uint16_t
{ 
    kC2S_Start = 1000,
    kC2S_Login,     // ¿Í»§¶ËµÇÂ¼

    kS2C_Start = 5000,
    kS2C_Login,
    kS2C_LoadCompleted,
};

}
}

#endif