#ifndef __NGSERVER_AUTOCALL_H_INCLUDE__
#define __NGSERVER_AUTOCALL_H_INCLUDE__

/*
*   AutoCall.h
*   消息的注册和调用机制
*/

/*  IDelegate 封装调用接口和调用方式
*   调用时只有一个参数：Decoder 这个Decoder是虚拟的解码器，消息响应函数的所有参数都从中读取，
*   它可以是ProtocolReader这样的真实解码器，也可以是其它任意类型，只要在Deletegate上提供对应的特例化，定制自己的Call函数对参数进行解码
*/

namespace NGServer{
namespace common{

template <typename Decoder>
class IDelegate
{
public:
    virtual ~IDelegate(){}
    virtual bool Call(Decoder& s) = 0;
};

/***********************************************************/
/*    默认的Delegate，所有参数都通过Decode全局函数解码得出    */
/***********************************************************/

// 0个参数的响应函数
template<typename Decoder, typename FuncT>
class Delegate0 : public IDelegate < Decoder >
{
    FuncT _func;

public:
    Delegate0(FuncT func) :
        _func(func){}

    bool Call(Decoder& s) override
    {
        _func();
        return true;
    }
};

// 1个参数的响应函数
template<typename Decoder, typename T1, typename FuncT>
class Delegate1 : public IDelegate < Decoder >
{
    FuncT _func;

public:
    Delegate1(FuncT func) :
        _func(func){}

    bool Call(Decoder& s) override
    {
        std::remove_const < std::remove_reference<T1>::type >::type t1;

        if (!Decode(s, t1))
            return false;

        _func(t1);
        return true;
    }
};

// 2个参数
template<typename Decoder, typename T1, typename T2, typename FuncT>
class Delegate2 : public IDelegate < Decoder >
{
    FuncT _func;

public:
    Delegate2(FuncT func) :
        _func(func){}

    bool Call(Decoder& s) override
    {
        std::remove_const< std::remove_reference<T1>::type >::type t1;
        std::remove_const< std::remove_reference<T2>::type >::type t2;

        if (!Decode(s, t1))
            return false;
        if (!Decode(s, t2))
            return false;

        _func(t1, t2);
        return true;
    }
};

// 3个参数
template<typename Decoder, typename T1, typename T2, typename T3, typename FuncT>
class Delegate3 : public IDelegate < Decoder >
{
    FuncT _func;

public:
    Delegate3(FuncT func) :
        _func(func){}

    bool Call(Decoder& s) override
    {
        std::remove_const< std::remove_reference<T1>::type >::type t1;
        std::remove_const< std::remove_reference<T2>::type >::type t2;
        std::remove_const< std::remove_reference<T3>::type >::type t3;

        if (!Decode(s, t1))
            return false;
        if (!Decode(s, t2))
            return false;
        if (!Decode(s, t3))
            return false;

        _func(t1, t2, t3);
        return true;
    }
};

/*******************/
/*   创建Delegate  */
/*******************/
template<typename Decoder, typename FuncT>
IDelegate<Decoder>* CreateDelegate0(FuncT f)
{
    return new Delegate0<Decoder, FuncT>(f);
}

template<typename Decoder, typename T1, typename FuncT>
IDelegate<Decoder>* CreateDelegate1(FuncT f)
{
    return new Delegate1<Decoder, T1, FuncT>(f);
}

template<typename Decoder, typename T1, typename T2, typename FuncT>
IDelegate<Decoder>* CreateDelegate2(FuncT f)
{
    return new Delegate2<Decoder, T1, T2, FuncT>(f);
}

template<typename Decoder, typename T1, typename T2, typename T3, typename FuncT>
IDelegate<Decoder>* CreateDelegate3(FuncT f)
{
    return new Delegate2<Decoder, T1, T2, T3, FuncT>(f);
}

/**************************************************/
/*   DelegateManager 管理所有响应函数的注册和回调   */
/**************************************************/
template<typename Decoder, size_t Capacity = 65535>
class DelegateManager
{

    typedef typename IDelegate<Decoder>* DelegatePtr;
    DelegatePtr _caller[Capacity];

public:
    DelegateManager()
    {
        memset(_caller, 0, sizeof(_caller));
    }

    ~DelegateManager()
    {
        for (size_t i = 0; i < Capacity; i++)
        {
            if (_caller[i] != nullptr)
            {
                delete _caller[i];
                _caller[i] = nullptr;
            }
        }
    }

    bool Call(uint16_t id, Decoder& s)
    {
        if (_caller[id] != nullptr)
            return _caller[id]->Call(s);
        else
            return false;
    }


    DelegatePtr Regist(uint16_t id, DelegatePtr dp)
    {
        if (_caller[id] != nullptr)
            delete _caller[id];

        _caller[id] = dp;
        return dp;
    }
    // 注册
public:

    // 完全匹配 全局函数
    template<typename R>
    DelegatePtr Regist(uint16_t id, R(*f)())
    {
        return Regist(id, CreateDelegate0<Decoder>(f));;
    }

    template<typename R, typename T1>
    DelegatePtr Regist(uint16_t id, R(*f)(T1))
    {
        return Regist(id, CreateDelegate1<Decoder, T1>(f));
    }

    template<typename R, typename T1, typename T2>
    DelegatePtr Regist(uint16_t id, R(*f)(T1, T2))
    {
        return Regist(id, CreateDelegate2<Decoder, T1, T2>(f));
    }

    template<typename R, typename T1, typename T2, typename T3>
    DelegatePtr Regist(uint16_t id, R(*f)(T1, T2, T3))
    {
        return Regist(id, CreateDelegate3<Decoder, T1, T2, T3>(f));
    }

    // 完全匹配 成员函数
    template<typename R, typename ObjT>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(), ObjT* obj)
    {
        std::function<R()> bindf = std::bind(f, obj);
        return Regist(id, CreateDelegate0<Decoder>(bindf));
    }

    template<typename R, typename ObjT, typename T1>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(T1), ObjT* obj)
    {
        std::function<R(T1)> bindf = std::bind(f, obj, std::placeholders::_1);
        return Regist(id, CreateDelegate1<Decoder, T1>(bindf));
    }

    template<typename R, typename ObjT, typename T1, typename T2>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(T1, T2), ObjT* obj)
    {
        std::function<R(T1, T2)> bindf = std::bind(f, obj, std::placeholders::_1, std::placeholders::_2);
        return Regist(id, CreateDelegate2<Decoder, T1, T2>(bindf));
    }

    template<typename R, typename ObjT, typename T1, typename T2, typename T3>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(T1, T2, T3), ObjT* obj)
    {
        std::function<R(T1, T2, T3)> bindf = std::bind(f, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return Regist(id, CreateDelegate3<Decoder, T1, T2, T3>(bindf));
    }

    // 不完全匹配 全局函数
    template<typename FuncT>
    DelegatePtr Regist0(uint16_t id, FuncT f)
    {
        return Regist(id, CreateDelegate0<Decoder, FuncT>(f));
    }

    template<typename T1, typename FuncT>
    DelegatePtr Regist1(uint16_t id, FuncT f)
    {
        return Regist(id, CreateDelegate1<Decoder, T1>(f));
    }

    template<typename T1, typename T2, typename FuncT>
    DelegatePtr Regist2(uint16_t id, FuncT f)
    {
        return Regist(id, CreateDelegate2<Decoder, T1, T2>(f));
    }

    template<typename T1, typename T2, typename T3, typename FuncT>
    DelegatePtr Regist3(uint16_t id, FuncT f)
    {
        return Regist(id, CreateDelegate3<Decoder, T1, T2, T3>(f));
    }

    // 不完全匹配 成员函数
    template<typename FuncT, typename ObjT>
    DelegatePtr Regist0(uint16_t id, FuncT f, ObjT* obj)
    {
        auto bindf = std::bind(f, obj);
        return Regist(id, CreateDelegate0<Decoder>(bindf));
    }

    template<typename T1, typename FuncT, typename ObjT>
    DelegatePtr Regist1(uint16_t id, FuncT f, ObjT* obj)
    {
        auto bindf = std::bind(f, obj, std::placeholders::_1);
        return Regist(id, CreateDelegate1<Decoder, T1>(bindf));
    }

    template<typename T1, typename T2, typename FuncT, typename ObjT>
    DelegatePtr Regist2(uint16_t id, FuncT f, ObjT* obj)
    {
        auto bindf = std::bind(f, obj, std::placeholders::_1, std::placeholders::_2);
        return Regist(id, CreateDelegate2<Decoder, T1, T2>(bindf));
    }

    template<typename T1, typename T2, typename T3, typename FuncT, typename ObjT>
    DelegatePtr Regist3(uint16_t id, FuncT f, ObjT* obj)
    {
        auto bindf = std::bind(f, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return Regist(id, CreateDelegate3<Decoder, T1, T2, T3>(bindf));
    }

    // 完全匹配  成员函数  该成员函数的this指针从Decoder中读取
    // 这里必须要使用bind函数 预留出this指针的位置
    template<typename R, typename ObjT>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)())
    {
        auto bindf = std::bind(f, placeholders::_1);
        return Regist(id, CreateDelegate1<Decoder, ObjT*>(bindf));
    }

    template<typename R, typename ObjT, typename T1>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(T1))
    {
        auto bindf = std::bind(f, placeholders::_1, placeholders::_2);
        return Regist(id, CreateDelegate2<Decoder, ObjT*, T1>(bindf));
    }

    template<typename R, typename ObjT, typename T1, typename T2>
    DelegatePtr Regist(uint16_t id, R(ObjT::*f)(T1, T2))
    {
        auto bindf = std::bind(f, placeholders::_1, placeholders::_2, placeholders::_3);
        return Regist(id, CreateDelegate3<Decoder, ObjT*, T1, T2>(bindf));
    }
};

}
}
#endif