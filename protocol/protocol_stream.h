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
*   获取指定对象序列化之后的大小 隐含加密等处理
*
*/
class ProtocolSize
{
public:
    // 外部接口 得到t序列化后的大小
    template<typename T>
    inline uint32_t GetSize(T& t)
    {
        // 全局模板推导
        return AutoMsgSize(*this, t);
    }

    // 内部接口 最终处理二进制数据 包括加密等 这里不做加密处理
    inline uint32_t GetSize(const void* ptr, uint32_t len)
    {
        return len;
    }

    // 对基本数据类型的特例化
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

    // 字符串
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
*   序列化基类  定义消息格式 一条消息对应一个ProtocolStream对象
*   包含内部缓冲 并且定义头部长度 整个消息体结构类似于:
*   消息长度 + 消息ID + 消息体
*/
class ProtocolStream
{
public:
    static const uint16_t kHeadLen = 2; // 头部长度 也就是消息长度 用两个字节表示
    static const uint16_t kMsgIdLen = 2;// 消息ID 用两个字节表示

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
*   序列化类 将数据序列化到缓冲区中
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

    // 编码消息 消息内部包含一个静态成员,指出消息ID 将其一并编码
    // 编码成功 返回消息编码后的长度 编码失败 返回0
    template< typename MsgT >
    uint16_t EncodeMsg(MsgT& t)
    {
        if (AutoEncode(*this, t))
        {
            return WriteHead(MsgT::msgid);
        }
        return 0;
    }

    // 实际写入二进制消息数据
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

    // 对基本数据类型的特例化
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

    // 对string的数据写入
    inline bool Write(const std::string& v)     { return WriteString(v); }
    inline bool Write(const std::wstring& v)    { return WriteString(v); }

    // 将消息长度和消息ID写入头部 返回缓冲区当前大小
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
            // 写入长度
            if (Write(&len, sizeof(len)) && len > 0)
            {
                // 写入数据
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
*   反序列化类
*   对于ProtocolReader 它读取的buf 一般只有消息ID和消息内容
*    而消息长度在外面读取 并且用于指定ProtocolReader缓冲区的大小
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

    // 读取二进制数据
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

    // 读取基本类型的数据
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

    // 对string的数据写入
    inline bool Read(std::string& v)     { return ReadString(v); }
    inline bool Read(std::wstring& v)    { return ReadString(v); }

    // 读取头部和消息ID
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

#pragma region 辅助函数 

// 辅助函数 用于方便生成自定义消息的 Encode Decode GetMsgSize 三件套

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