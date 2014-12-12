#ifndef __NGSERVER_PROTOCOL_STREAM_H_INCLUDE__
#define __NGSERVER_PROTOCOL_STREAM_H_INCLUDE__

#include <stdint.h>
#include <string>
#include <assert.h>
#include "serialization.h"

namespace NGServer
{
namespace protocol
{

/*
*   Protocolsize
*   ��ȡָ���������л�֮��Ĵ�С �������ܵȴ���
*
*/
class ProtocolSize
{
public:
    // �ⲿ�ӿ� �õ�t���л���Ĵ�С
    template<typename T>
    inline uint32_t GetSize(T& t)
    {
        // ȫ��ģ���Ƶ�
        return AutoMsgSize(*this, t);
    }

    // �ڲ��ӿ� ���մ������������ �������ܵ� ���ﲻ�����ܴ���
    inline uint32_t GetSize(const void* ptr, uint32_t len)
    {
        return len;
    }

    // �Ի����������͵�������
    inline uint32_t GetSize(char v)    { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(int8_t v)  { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(uint8_t v) { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(int16_t v) { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(uint16_t v){ return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(int32_t v) { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(uint32_t v){ return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(int64_t v) { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(uint64_t v){ return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(float v)   { return GetSize((const void*)(&v), sizeof(v)); }
    inline uint32_t GetSize(double v)  { return GetSize((const void*)(&v), sizeof(v)); }

    // �ַ���
    inline uint32_t GetSize(const std::string& v){ return GetStringSize(v); }
    inline uint32_t GetSize(const std::wstring& v){ return GetStringSize(v); }

private:
    inline uint32_t GetStringSize(const std::string& v)
    {
        uint16_t len = static_cast<uint16_t>(v.size());
        return GetSize(len) + GetSize((const void*)(v.c_str()), len*sizeof(char));
    }

    inline uint32_t GetStringSize(const std::wstring& v)
    {
        uint16_t len = static_cast<uint16_t>(v.size());
        return GetSize(len) + GetSize((const void*)(v.c_str()), len*sizeof(wchar_t));
    }
};

/*
*   ProtocolStream
*   ���л�����  ������Ϣ��ʽ һ����Ϣ��Ӧһ��ProtocolStream����
*   �����ڲ����� ���Ҷ���ͷ������ ������Ϣ��ṹ������:
*   ��Ϣ���� + ��ϢID + ��Ϣ��
*/
class ProtocolStream
{
public:
    static const uint16_t kHeadLen = 2; // ͷ������ Ҳ������Ϣ���� �������ֽڱ�ʾ
    static const uint16_t kMsgIdLen = 2;// ��ϢID �������ֽڱ�ʾ

    inline uint32_t Capacity() const { return _capacity; }
    inline uint32_t Remain() const { return _capacity - _offset; }

protected:
    ProtocolStream(char* buf, uint32_t len) :
        _capacity(len), _buf(buf), _offset(kHeadLen+kMsgIdLen)
    {
    }

protected:
    char* const _buf;
    uint32_t _offset;
    const uint32_t _capacity;
};

/*
*   ProtocolWriter
*   ���л��� ���������л�����������
*/
class ProtocolWriter : public ProtocolStream
{
public:
    ProtocolWriter(char* buf, uint32_t len) :
        ProtocolStream(buf, len){}

    template < typename T >
    bool Encode(T& t)
    {
        return AutoEncode(*this, t);
    }

    // ������Ϣ ��Ϣ�ڲ�����һ����̬��Ա,ָ����ϢID ����һ������
    // ����ɹ� ������Ϣ�����ĳ��� ����ʧ�� ����0
    template< typename MsgT >
    uint16_t EncodeMsg(MsgT& t)
    {
        if (AutoEncode(*this, t))
        {
            return WriteHead(MsgT::msgid);
        }
        return 0;
    }

    // ʵ��д���������Ϣ����
    bool Write(const void* data, uint32_t len)
    {
        if (Remain() >= len)
        {
            memcpy(_buf + _offset, data, len);
            _offset += len;
            return true;
        }
        return false;
    }

    // �Ի����������͵�������
    inline bool Write(char v)    { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(int8_t v)  { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(uint8_t v) { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(int16_t v) { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(uint16_t v){ return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(int32_t v) { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(uint32_t v){ return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(int64_t v) { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(uint64_t v){ return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(float v)   { return Write((const void*)(&v), sizeof(v)); }
    inline bool Write(double v)  { return Write((const void*)(&v), sizeof(v)); }

    // ��string������д��
    inline bool Write(const std::string& v)     { return WriteString(v); }
    inline bool Write(const std::wstring& v)    { return WriteString(v); }

    // ����Ϣ���Ⱥ���ϢIDд��ͷ�� ���ػ�������ǰ��С
    uint32_t WriteHead(uint16_t msgid)
    {
        uint16_t* p = (uint16_t*)_buf;
        *p = _offset;
        *(p + 1) = msgid;
        return _offset;
    }

private:

    bool WriteString(const std::string& v)
    {
        uint16_t len = static_cast<uint16_t>(v.size());
        if (Remain() > sizeof(len) + len*sizeof(char))
        {
            // д�볤��
            if (Write(&len, sizeof(len)) && len > 0)
            {
                // д������
                return Write(v.c_str(), len*sizeof(char));
            }
            return true;
        }
        return false;
    }

    bool WriteString(const std::wstring& v)
    {
        uint16_t len = static_cast<uint16_t>(v.size());
        if (Remain() > sizeof(len) + len*sizeof(wchar_t))
        {
            if (Write(&len, sizeof(len)) && len > 0)
            {
                return Write(v.c_str(), len*sizeof(wchar_t));
            }
        }
    }

};

/*
*   ProtocolReader
*   �����л���
*   ����ProtocolReader ����ȡ��buf һ��ֻ����ϢID����Ϣ����
*    ����Ϣ�����������ȡ ��������ָ��ProtocolReader�������Ĵ�С
*/
class ProtocolReader : public ProtocolStream
{
public:
    ProtocolReader(char* buf, uint32_t len) :
        ProtocolStream(buf, len){}

    template<typename T>
    bool Decode(T& t)
    {
        return AutoDecode(*this, t);
    }

    // ��ȡ����������
    bool Read(void* ptr, uint32_t len)
    {
        if (Remain() >= len)
        {
            memcpy(ptr, _buf + _offset, len);
            _offset += len;
            return true;
        }
        return false;
    }

    // ��ȡ�������͵�����
    inline bool Read(char& v)    { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(int8_t& v)  { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(uint8_t& v) { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(int16_t& v) { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(uint16_t& v){ return Read((void*)(&v), sizeof(v)); }
    inline bool Read(int32_t& v) { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(uint32_t& v){ return Read((void*)(&v), sizeof(v)); }
    inline bool Read(int64_t& v) { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(uint64_t& v){ return Read((void*)(&v), sizeof(v)); }
    inline bool Read(float& v)   { return Read((void*)(&v), sizeof(v)); }
    inline bool Read(double& v)  { return Read((void*)(&v), sizeof(v)); }

    // ��string������д��
    inline bool Read(std::string& v)     { return ReadString(v); }
    inline bool Read(std::wstring& v)    { return ReadString(v); }

    // ��ȡͷ������ϢID
    inline uint16_t ReadHead()
    {
        uint16_t* h = (uint16_t*)_buf;
        return *h;
    }
    inline uint16_t ReadMsgId()
    {
        uint16_t* h = (uint16_t*)_buf;
        return *(h + 1);
    }
private:
    bool ReadString(std::string& v)
    {
        uint16_t len;
        if (Read(len))
        {
            v.clear();
            if (len > 0)
            {
                assert(Remain() >= len*sizeof(char));
                v.append((const char*)(_buf + _offset), len);
                _offset += len;
            }
            return true;
        }
        return false;
    }

    bool ReadString(std::wstring& v)
    {
        uint16_t len;
        if (Read(len))
        {
            v.clear();
            if (len > 0)
            {
                assert(Remain() >= len*sizeof(wchar_t));
                v.append((const wchar_t*)(_buf + _offset), len);
                _offset += len*sizeof(wchar_t);
            }
        }
    }

};

}
}

#pragma region �������� 

// �������� ���ڷ��������Զ�����Ϣ�� Encode Decode GetMsgSize ������

#define AUTOCUSTOMMSG1(T, v1) \
    bool Encode(ProtocolWriter& s, const T& t){ \
        return AutoEncode(s, t.v1); } \
    \
    bool Decode(ProtocolReader& s,  T& t){ \
        return AutoDecode(s, t.v1); } \
    \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t ){ \
        return AutoMsgSize(s, t.v1); } 

#define AUTOCUSTOMMSG2(T, v1, v2) \
    bool Encode(ProtocolWriter& s, const T& t){ \
        return AutoEncode(s, t.v1) && AutoEncode(s, t.v2); } \
    \
    bool Decode(ProtocolReader& s,  T& t){ \
        return AutoDecode(s, t.v1) && AutoDecode(s, t.v2); } \
    \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t ){ \
        return AutoMsgSize(s, t.v1) + AutoMsgSize(s, t.v2); } 

#define AUTOCUSTOMMSG3(T, v1, v2, v3) \
    bool Encode(ProtocolWriter& s, const T& t){ \
        return AutoEncode(s, t.v1) && AutoEncode(s, t.v2) && AutoEncode(s, t.v3); } \
    \
    bool Decode(ProtocolReader& s,  T& t){ \
        return AutoDecode(s, t.v1) && AutoDecode(s, t.v2) && AutoDecode(s, t.v3); } \
    \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t ){ \
        return AutoMsgSize(s, t.v1) + AutoMsgSize(s, t.v2) + AutoMsgSize(s, t.v3); } 

#define AUTOCUSTOMMSG4(T, v1, v2, v3, v4) \
    bool Encode(ProtocolWriter& s, const T& t){ \
        return AutoEncode(s, t.v1) && AutoEncode(s, t.v2) && AutoEncode(s, t.v3) && AutoEncode(s, t.v4); } \
    \
    bool Decode(ProtocolReader& s,  T& t){ \
        return AutoDecode(s, t.v1) && AutoDecode(s, t.v2) && AutoDecode(s, t.v3) && AutoDecode(s, t.v4); } \
    \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t ){ \
        return AutoMsgSize(s, t.v1) + AutoMsgSize(s, t.v2) + AutoMsgSize(s, t.v3) + AutoMsgSize(s, t.v4); } 

#define AUTOCUSTOMMSG5(T, v1, v2, v3, v4, v5) \
    bool Encode(ProtocolWriter& s, const T& t){ \
        return AutoEncode(s, t.v1) && AutoEncode(s, t.v2) && AutoEncode(s, t.v3) && AutoEncode(s, t.v4) && AutoEncode(s, t.v5); } \
    \
    bool Decode(ProtocolReader& s,  T& t){ \
        return AutoDecode(s, t.v1) && AutoDecode(s, t.v2) && AutoDecode(s, t.v3) && AutoDecode(s, t.v4) && AutoDecode(s, t.v5); } \
    \
    uint32_t GetMsgSize(ProtocolSize& s, const T& t ){ \
        return AutoMsgSize(s, t.v1) + AutoMsgSize(s, t.v2) + AutoMsgSize(s, t.v3) + AutoMsgSize(s, t.v4) + AutoMsgSize(s, t.v5); } 


#endif