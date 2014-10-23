#ifndef __NGSERVER_AGENTCALL_H_INCLUDE__
#define __NGSERVER_AGENTCALL_H_INCLUDE__

#include <type_traits>
#include <utility>

/*
*   AgentCall.h
*   ��Ϣ��ע��͵��û���
*   AgentCall ע��͵��õ�һ������Ϊָ��������������������Э���ж�ȡ�ĵ��÷�ʽ
*   ���£�
*       Func(T1, T2, T3, ...) ����T1Ϊ����ʱָ��  T2, T3, ... ����Э���ж�ȡ 
*       A::Func(T1, T2, T3, ...) ���ֳ�Ա�������÷�ʽ Ҳ��AgentCallʵ��
*
*/

/*  IAgent ��װ���ýӿں͵��÷�ʽ
*   T1: �ڵ���ʱ����ĵ�һ������. ���� Player
*   Decoder: Э������� ���ڶ�ȡʣ�����������. ���� ProtocolReader
*/
namespace NGServer
{
namespace common
{

template<typename T1, typename Decoder>
class IAgent
{
public:
    virtual bool Call(T1&& t1, Decoder& decoder) = 0;
};

#pragma region IAgentʵ�� ȫ�ֺ�������
// ������ʽ�� func(t1)
template<typename FuncT, typename T1, typename Decoder>
class Agent1 : public IAgent < T1, Decoder >
{
public:
    Agent1(FuncT f)
        :_func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        _func(std::forward<T1>(t1));
        return true;
    }

private:
    FuncT _func;
};


// ������ʽ: func(t1, t2)
template<typename FuncT, typename T1, typename T2, typename Decoder>
class Agent2 : public IAgent < T1, Decoder >
{
public:
    Agent2(FuncT f)
        :_func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        _func(std::forward<T1>(t1), t2);
        return true;
    }

private:
    FuncT _func;
};

// ������ʽ func(t1, t2, t3)
template<typename FuncT, typename T1, typename T2, typename T3, typename Decoder>
class Agent3 : public IAgent < T1, Decoder >
{
public:
    Agent3(FuncT f)
        :_func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        _func(std::forward<T1>(t1), t2, t3);
        return true;
    }

private:
    FuncT _func;
};

// ������ʽ func(t1, t2, t3, t4)
template<typename FuncT, typename T1, typename T2, typename T3, typename T4, typename Decoder>
class Agent4 : public IAgent < T1, Decoder >
{
public:
    Agent4(FuncT f)
        :_func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        std::remove_const<T4>::type t4;
        if (!Decode(decoder, t4))
            return false;

        _func(std::forward<T1>(t1), t2, t3, t4);
        return true;
    }

private:
    FuncT _func;
};

// ������ʽ func(t1, t2, t3, t4, t5)
template<typename FuncT, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
class Agent5 : public IAgent < T1, Decoder >
{
public:
    Agent5(FuncT f)
        :_func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        std::remove_const<T4>::type t4;
        if (!Decode(decoder, t4))
            return false;

        std::remove_const<T5>::type t5;
        if (!Decode(decoder, t5))
            return false;

        _func(std::forward<T1>(t1), t2, t3, t4, t5);
        return true;
    }

private:
    FuncT _func;
};
#pragma endregion

#pragma region IAgentʵ�� ��Ա��������

template<typename ObjT, typename FuncT, typename T1, typename Decoder>
class AgentObj1 : public IAgent<T1, Decoder>
{
public:
    AgentObj1(ObjT* obj, FuncT f)
        :_obj(obj), _func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        (_obj->*_func)(std::forward<T1>(t));
        return true;
    }

private:
    ObjT* _obj;
    FuncT _func;
};

template<typename ObjT, typename FuncT, typename T1, typename T2, typename Decoder>
class AgentObj2 : public IAgent<T1, Decoder> 
{
public:
    AgentObj2(ObjT* obj, FuncT f)
        : _obj(obj), _func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        (_obj->*_func)(std::forward<T1>(t1), t2);
        return true;
    }

private:
    ObjT* _obj;
    FuncT _func;
};

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename Decoder>
class AgentObj3 : public IAgent < T1, Decoder >
{
public:
    AgentObj3(ObjT* obj, FuncT f)
        : _obj(obj), _func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        (_obj->*_func)(std::forward<T1>(t1), t2, t3);
        return true;
    }

private:
    ObjT* _obj;
    FuncT _func;
};

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4, typename Decoder>
class AgentObj4 : public IAgent < T1, Decoder >
{
public:
    AgentObj4(ObjT* obj, FuncT f)
        : _obj(obj), _func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        std::remove_const<T4>::type t4;
        if (!Decode(decoder, t4))
            return false;

        (_obj->*_func)(std::forward<T1>(t1), t2, t3, t4);
        return true;
    }

private:
    ObjT* _obj;
    FuncT _func;
};

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
class AgentObj5 : public IAgent < T1, Decoder >
{
public:
    AgentObj5(ObjT* obj, FuncT f)
        : _obj(obj), _func(f){}

    bool Call(T1&& t1, Decoder& decoder) override
    {
        std::remove_const<T2>::type t2;
        if (!Decode(decoder, t2))
            return false;

        std::remove_const<T3>::type t3;
        if (!Decode(decoder, t3))
            return false;

        std::remove_const<T4>::type t4;
        if (!Decode(decoder, t4))
            return false;

        std::remove_const<T5>::type t5;
        if (!Decode(decoder, t5))
            return false;

        (_obj->*_func)(std::forward<T1>(t1), t2, t3, t4, t5);
        return true;
    }

private:
    ObjT* _obj;
    FuncT _func;
};

#pragma endregion

#pragma region ȫ�ֺ��� ��������ȫƥ��

template<typename FuncT, typename T1, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent1(FuncT f)
{
    return new Agent1<FuncT, T1, Decoder>(f);
}

template<typename FuncT, typename T1, typename T2, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent2(FuncT f)
{
    return new Agent2<FuncT, T1, 
                       std::remove_reference<T2>::type, 
                       Decoder>(f);
}

template<typename FuncT, typename T1, typename T2, typename T3, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent3(FuncT f)
{
    return new Agent3<FuncT, T1, 
                       std::remove_reference<T2>::type, 
                       std::remove_reference<T3>::type, 
                       Decoder>(f);
}

template<typename FuncT, typename T1, typename T2, typename T3, typename T4, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent4(FuncT f)
{
    return new Agent4<FuncT, T1, 
                       std::remove_reference<T2>::type, 
                       std::remove_reference<T3>::type, 
                       std::remove_reference<T4>::type, 
                       Decoder>(f);
}

template<typename FuncT, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent5(FuncT f)
{
    return new Agent5<FuncT, T1, 
                       std::remove_reference<T2>::type, 
                       std::remove_reference<T3>::type, 
                       std::remove_reference<T4>::type, 
                       std::remove_reference<T5>::type, 
                       Decoder>(f);
}

#pragma endregion

#pragma region ȫ�ֺ��� ������ȫƥ��

template<typename R, typename T1, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent(R(*f)(T1))
{
    return new Agent1<R(*)(T1), T1, Decoder>(f);
}

template<typename R, typename T1, typename T2, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent(R(*f)(T1, T2))
{
    return new Agent2<R(*)(T1, T2), T1,
        std::remove_reference<T2>::type,
        Decoder>(f);
}

template<typename R, typename T1, typename T2, typename T3, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent(R(*f)(T1, T2, T3))
{
    return new Agent3<R(*)(T1, T2, T3), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        Decoder>(f);
}

template<typename R, typename T1, typename T2, typename T3, typename T4, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent(R(*f)(T1, T2, T3, T4))
{
    return new Agent4<R(*)(T1, T2, T3, T4), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        Decoder>(f);
}

template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
inline IAgent<T1, Decoder>* CreateAgent(R(*f)(T1, T2, T3, T4, T5))
{
    return new Agent5<R(*)(T1, T2, T3, T4, T5), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        std::remove_reference<T5>::type,
        Decoder>(f);
}

#pragma endregion

#pragma region ��Ա���� ��������ȫƥ��

template<typename ObjT, typename FuncT, typename T1, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent1(ObjT* obj, FuncT func)
{
    return new AgentObj1<ObjT, FuncT, T1, Decoder>(obj, func);
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent2(ObjT* obj, FuncT func)
{
    return new AgentObj2<ObjT, FuncT, T1, 
        std::remove_reference<T2>::type, 
        Decoder>(obj, func);
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent3(ObjT* obj, FuncT func)
{
    return new AgentObj3<ObjT, FuncT, T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        Decoder>(obj, func);
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent4(ObjT* obj, FuncT func)
{
    return new AgentObj4<ObjT, FuncT, T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        Decoder>(obj, func);
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent5(ObjT* obj, FuncT func)
{
    return new AgentObj5<ObjT, FuncT, T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        std::remove_reference<T5>::type,
        Decoder>(obj, func);
}

#pragma endregion

#pragma region ��Ա���� ������ȫƥ��

template<typename R, typename ObjT, typename T1, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent(R(ObjT::*f)(T1), ObjT* obj)
{
    return new AgentObj1<ObjT, R(ObjT::*f)(T1), T1, Decoder>(obj, f);
}

template<typename R, typename ObjT, typename T1, typename T2, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent(R(ObjT::*f)(T1, T2), ObjT* obj)
{
    return new AgentObj2<ObjT, R(ObjT::*f)(T1, T2), T1,
        std::remove_reference<T2>::type,
        Decoder>(obj, f);
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent(R(ObjT::*f)(T1, T2, T3), ObjT* obj)
{
    return new AgentObj3<ObjT, R(ObjT::*f)(T1, T2, T3), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        Decoder>(obj, f);
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3, typename T4, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent(R(ObjT::*f)(T1, T2, T3, T4), ObjT* obj)
{
    return new AgentObj3<ObjT, R(ObjT::*f)(T1, T2, T3, T4), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        Decoder>(obj, f);
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3, typename T4, typename T5, typename Decoder>
inline IAgent<T1, Decoder> CreateAgent(R(ObjT::*f)(T1, T2, T3, T4, T5), ObjT* obj)
{
    return new AgentObj3<ObjT, R(ObjT::*f)(T1, T2, T3, T4, T5), T1,
        std::remove_reference<T2>::type,
        std::remove_reference<T3>::type,
        std::remove_reference<T4>::type,
        std::remove_reference<T5>::type,
        Decoder>(obj, f);
}

#pragma endregion

#pragma region AgentManager

/*
*   AgentManager
*   T ���ýӿڵĵ�һ������  �ڵ���ʱָ��
*   Decoder Э������� ���ж���������������
*   �������е�Agent 
        ��Ϣע�᣺��һ����ϢID��һ��Agent��Ӧ���� 
        �ӿ�ת����ͨ����ϢID����Agent::Call�ӿ�
*/
template<typename T, typename Decoder>
class AgentManager
{
    const static int capacity = 256 * 256;
    typedef IAgent<T, Decoder>* AgentPtr;

public:
    AgentManager()
    {
        memset(_caller, 0, sizeof(_caller));
    }

    ~AgentManager()
    {
        for (int i = 0; i < capacity; i++)
        {
            AgentPtr p = _caller[i];
            delete p;
            _caller[i] = nullptr;
        }
    }

public:
    bool Call(uint16_t msgid, T&& t, Decoder& decoder)
    {
        AgentPtr agent = _caller[msgid];
        if (agent != nullptr)
        {
            return agent->Call(t, decoder);
        }
        return false;
    }

    template<typename R, typename T1, typename T2>
    uint16_t RegistMsg(R(*f)(T1, T2))
    {
        typedef typename std::remove_reference<T2>::type TT2;
        const uint16_t msgid = static_cast<uint16_t>(std::remove_const<TT2>::type::msgid);
        Regist(msgid, CreateAgent<R, T1, T2, Decoder>(f));
        return msgid;
    }

private:
    AgentPtr Regist(uint16_t msgid, AgentPtr agent)
    {
        AgentPtr oldAgent = _caller[msgid];
        _caller[msgid] = agent;
        if (oldAgent != nullptr)
            delete oldAgent;
        return agent;
    }

private:
    AgentPtr _caller[capacity];

public:
#pragma region ע�᷽ʽ�� ȫ�ֺ��� ����ȫƥ�� 
    template<typename T1, typename FuncT>
    inline AgentPtr Regist1(uint16_t id, FuncT f)
    {
        return Regist(id, CreateAgent1<FuncT, T1, Decoder>(f));
    }

    template<typename T1, typename T2, typename FuncT>
    inline AgentPtr Regist2(uint16_t id, FuncT f)
    {
        return Regist(id, CreateAgent2<FuncT, T1, T2, Decoder>(f));
    }

    template<typename T1, typename T2, typename T3, typename FuncT>
    inline AgentPtr Regist3(uint16_t id, FuncT f)
    {
        return Regist(id, CreateAgent3<FuncT, T1, T2, T3, Decoder>(f));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename FuncT>
    inline AgentPtr Regist4(uint16_t id, FuncT f)
    {
        return Regist(id, CreateAgent4<FuncT, T1, T2, T3, T4, Decoder>(f));
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename FuncT>
    inline AgentPtr Regist5(uint16_t id, FuncT f)
    {
        return Regist(id, CreateAgent5<FuncT, T1, T2, T3, T4, T5, Decoder>(f));
    }

#pragma endregion

#pragma region ע�᷽ʽ����Ա����  ����ȫƥ��

template<typename ObjT, typename FuncT, typename T1>
AgentPtr Regist1(uint16_t id, FuncT f, ObjT* obj)
{
    return Regist(id, CreateAgent1<ObjT, FuncT, T1, Decoder>(obj, f));
}

template<typename ObjT, typename FuncT, typename T1, typename T2>
AgentPtr Regist2(uint16_t id, FuncT f, ObjT* obj)
{
    return Regist(id, CreateAgent2<ObjT, FuncT, T1, T2, Decoder>(obj, f));
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3>
AgentPtr Regist3(uint16_t id, FuncT f, ObjT* obj)
{
    return Regist(id, CreateAgent3<ObjT, FuncT, T1, T2, T3, Decoder>(obj, f));
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4>
AgentPtr Regist4(uint16_t id, FuncT f, ObjT* obj)
{
    return Regist(id, CreateAgent4<ObjT, FuncT, T1, T2, T3, T4, Decoder>(obj, f));
}

template<typename ObjT, typename FuncT, typename T1, typename T2, typename T3, typename T4, typename T5>
AgentPtr Regist4(uint16_t id, FuncT f, ObjT* obj)
{
    return Regist(id, CreateAgent5<ObjT, FuncT, T1, T2, T3, T4, T5, Decoder>(obj, f));
}

#pragma endregion

#pragma region ע�᷽ʽ�� ȫ�ֺ���  ��ȫƥ��
    template<typename R, typename T1>
    inline AgentPtr Regist(uint16_t id, R(*f)(T1))
    {
        return Regist(id, CreateAgent<R, T1, Decoder>(f));
    }

    template<typename R, typename T1, typename T2>
    inline AgentPtr Regist(uint16_t id, R(*f)(T1, T2))
    {
        return Regist(id, CreateAgent<R, T1, T2, Decoder>(f));
    }

    template<typename R, typename T1, typename T2, typename T3>
    inline AgentPtr Regist(uint16_t id, R(*f)(T1, T2, T3))
    {
        return Regist(id, CreateAgent<R, T1, T2, T3, Decoder>(f));
    }

    template<typename R, typename T1, typename T2, typename T3, typename T4>
    inline AgentPtr Regist(uint16_t id, R(*f)(T1, T2, T3, T4))
    {
        return Regist(id, CreateAgent<R, T1, T2, T3, T4, Decoder>(f));
    }

    template<typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
    inline AgentPtr Regist(uint16_t id, R(*f)(T1, T2, T3, T4, T5))
    {
        return Regist(id, CreateAgent<R, T1, T2, T3, T4, T5, Decoder>(f));
    }
#pragma endregion

#pragma region ע�᷽ʽ����Ա���� ��ȫƥ��

template<typename R, typename ObjT, typename T1>
AgentPtr Regist(uint16_t id, R(ObjT::*f)(T1), ObjT* obj)
{
    return Regist(id, CreateAgent<R, ObjT, T1, Decoder>(f));
}

template<typename R, typename ObjT, typename T1, typename T2>
AgentPtr Regist(uint16_t id, R(ObjT::*f)(T1, T2), ObjT* obj)
{
    return Regist(id, CreateAgent<R, ObjT, T1, T2, Decoder>(f));
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3>
AgentPtr Regist(uint16_t id, R(ObjT::*f)(T1, T2, T3), ObjT* obj)
{
    return Regist(id, CreateAgent<R, ObjT, T1, T2, T3, Decoder>(f));
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3, typename T4>
AgentPtr Regist(uint16_t id, R(ObjT::*f)(T1, T2, T3, T4), ObjT* obj)
{
    return Regist(id, CreateAgent<R, ObjT, T1, T2, T3, T4, Decoder>(f));
}

template<typename R, typename ObjT, typename T1, typename T2, typename T3, typename T4, typename T5>
AgentPtr Regist(uint16_t id, R(ObjT::*f)(T1, T2, T3, T4, T5), ObjT* obj)
{
    return Regist(id, CreateAgent<R, ObjT, T1, T2, T3, T4, T5, Decoder>(f));
}

#pragma endregion
};


#pragma endregion

}
}

#endif