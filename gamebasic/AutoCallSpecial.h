#ifndef __NGSERVER_PROXYCALL_H_INCLUDE__
#define __NGSERVER_PROXYCALL_H_INCLUDE__

#include "../common/AutoCall.h"
#include "../gamebasic/Message.h"
#include "../protocol/protocol.h"

using namespace NGServer::protocol;
using namespace NGServer::common;
/*
* ProxyCall.h ������AutoCall�е�Delegate
*/

/***********************************************/
/*    ������Decoder: InsideMessage �ĵ��÷�ʽ   */
/***********************************************/
// ��������
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

// ��һ������ ��ΪInsideMessage�е���Ϣ����
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
/*   ������ Decoder: std::pair<T1, InsideMessage> T1����Ӧ�����ĵ�һ������  */
/**************************************************************************/


// ֻ��T1һ������
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

// ���������� ��һ������ΪT1 �ڶ�������ΪInsideMessage�е���Ϣ��
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
/*   ������Decoder: std::pair<T1, ProtocolReader&> T1����Ӧ�����ĵ�һ������ ����������ProtocolReader�ж�ȡ */
/*******************************************************************************************************/
// ��һ������ T1
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

// ����������  ��һ������ΪT1 �ڶ���������ProtocolReader�ж�ȡ
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