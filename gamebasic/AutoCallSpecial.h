#ifndef __NGSERVER_PROXYCALL_H_INCLUDE__
#define __NGSERVER_PROXYCALL_H_INCLUDE__

#include "../common/AutoCall.h"
#include "../gamebasic/Message.h"
#include "../protocol/protocol.h"

using namespace NGServer::protocol;
using namespace NGServer::common;
/*
* ProxyCall.h 特例化AutoCall中的Delegate
*/

/***********************************************/
/*    特例化Decoder: InsideMessage 的调用方式   */
/***********************************************/
// 不带参数
template<typename FuncT>
class Delegate0<InsideMessage, FuncT> : public IDelegate < InsideMessage > 
{
    FuncT _func;

public:
    Delegate0(FuncT f) :
        _func(f){}

    bool Call(InsideMessage& s) override
    {
        _func();
        return true;
    }
};

// 带一个参数 即为InsideMessage中的消息数据
template<typename T1, typename FuncT>
class Delegate1<InsideMessage, T1, FuncT> :public IDelegate < InsideMessage > 
{
    FuncT _func;

public:
    Delegate1(FuncT f) :
        _func(f){}

    bool Call(InsideMessage& s) override
    {
        typedef typename std::remove_const< std::remove_reference<T1>::type >::type TT1;
        typedef InsideMessageT< TT1 > MsgType;
        MsgType* msg = dynamic_cast<MsgType*>(&s);
        if (msg == nullptr)
        {
            assert(0);
            return false;
        }

        _func(msg->_data);

        return true;
    }
};

/**************************************************************************/
/*   特例化 Decoder: std::pair<T1, InsideMessage> T1是响应函数的第一个参数  */
/**************************************************************************/


// 只带T1一个参数
template<typename T1, typename FuncT>
class Delegate1<std::pair<T1, InsideMessage*>, T1, FuncT> : public IDelegate < std::pair<T1, InsideMessage*> > 
{
    FuncT _func;

public:
    Delegate1(FuncT f) :
        _func(f){}

    bool Call(std::pair<T1, InsideMessage*>& s) override
    {
        _func(s.first);
        return true;
    }
};

// 带两个参数 第一个参数为T1 第二个参数为InsideMessage中的消息体
template<typename T1, typename T2, typename FuncT>
class Delegate2<std::pair<T1, InsideMessage*>, T1, T2, FuncT> : public IDelegate < std::pair<T1, InsideMessage*> > 
{
    FuncT _func;

public:
    Delegate2(FuncT f) :
        _func(f){}

    bool Call(std::pair<T1, InsideMessage*>&s) override
    {
        typedef typename std::remove_const< std::remove_reference<T2>::type >::type TT2;
        typedef typename InsideMessageT<TT2> MsgType;

        MsgType* msg = dynamic_cast<MsgType*>(s.second);
        if (msg == nullptr)
        {
            assert(0);
            return false;
        }

        _func(s.first, msg->_data);
        return true;
    }
};

/*******************************************************************************************************/
/*   特例化Decoder: std::pair<T1, ProtocolReader&> T1是响应函数的第一个参数 其他参数从ProtocolReader中读取 */
/*******************************************************************************************************/
// 带一个参数 T1
template<typename T1, typename FuncT>
class Delegate1<std::pair<T1, ProtocolReader&>, T1, FuncT> : public IDelegate < std::pair<T1, ProtocolReader&> >
{
    FuncT _func;

public:
    Delegate1(FuncT f) :
        _func(f){}

    bool Call(std::pair<T1, ProtocolReader&>& s) override
    {
        _func(s.first);
        return true;
    }
};

// 带两个参数  第一个参数为T1 第二个参数从ProtocolReader中读取
template < typename T1, typename T2, typename FuncT > 
class Delegate2<std::pair<T1, ProtocolReader&>, T1, T2, FuncT> : public IDelegate < std::pair<T1, ProtocolReader&> > 
{
    FuncT _func;

public:
    Delegate2(FuncT f) :
        _func(f){}

    bool Call(std::pair<T1, ProtocolReader&>& s) override
    {
        std::remove_const< std::remove_reference<T2>::type >::type t2;

        if (!Decode(s.second, t2))
            return false;

        _func(s.first, t2);
        return true;
    }
};

#endif