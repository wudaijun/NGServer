#include "serialization.h"
#include "customize_protocol.h"

namespace NGServer
{
namespace protocol
{

AUTOCUSTOMMSG2(C2S_Login, owner, pwd);

AUTOCUSTOMMSG1(S2C_LoadCompleted, teststr);

}
}