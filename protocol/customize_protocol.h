#ifndef __NGSERVER_CUSTOMIZE_PROTOCOL_H_INCLUDE__
#define __NGSERVER_CUSTOMIZE_PROTOCOL_H_INCLUDE__

#include "protocol_stream.h"
#include "msgid.h"

#include <string>
#include <vector>

namespace NGServer
{
namespace protocol
{

#define CUSTOMMSG(T) \
    struct T;        \
    bool Encode(ProtocolWriter& s, const T& t); \
    bool Decode(ProtocolReader& s, T& t); \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t); \
    struct T

CUSTOMMSG(C2S_Login)
{
    
    NETMSGID(kC2S_Login);
    std::string owner;
    std::string pwd;

};

CUSTOMMSG(S2C_LoadCompleted)
{
    NETMSGID(kS2C_LoadCompleted);
    std::string teststr;
};

}
}

#endif