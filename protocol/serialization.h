#ifndef __NGSERVER_SERIALIZATION_H_INCLUDE__
#define __NGSERVER_SERIALIZATION_H_INCLUDE__

/*
*   serialization.h  Created : 2014-10-14 afternoon
*   编解码全局模板推导 在ProtocolReader ProtocolWriter中调用
*   该文件通过对数据的模板推导和型别识别 最终将复杂的数据类型分解为简单的数据类型
*   并且通过 ProtocolReader / ProtocolWriter 的 Read / Write 来执行写入和读出
*
*/

#include<string>
#include<stdint.h>
#include<vector>
#include<list>

namespace NGServer
{
namespace protocol
{


#pragma region 编码 Encode

        // 默认编码函数  针对于用户自定义的POD类型
        template<typename S, typename T>
        bool Encode(S& s, const T& t)
        {
            static_assert(std::is_trivial<T>::value, "not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "not standard layout. need to customize");
            return s.Write((const void*)&t, sizeof(t));
        }

        // 对简单容器的特例化
        template<typename S>
        bool Encode(S& s, const std::string& t)
        {
            return s.Write(t);
        }

        template<typename S>
        bool Encode(S& s, const std::wstring& t)
        {
            return s.Write(t);
        }

        template<typename S, typename T>
        bool Encode(S& s, const std::vector<T>& t)
        {
            return EncodeArray(s, t);
        }

        template<typename S, typename T>
        bool Encode(S& s, const std::list<T>& t)
        {
            return EncodeArray(s, t);
        }

        // 编码动态长度的容器 ( list . vector)
        template<typename S, typename T>
        bool EncodeArray(S& s, const T& t)
        {
            uint16_t size = static_cast<uint16_t>(t.size());
            if (s.Write(size))
            {
                for (auto& item : t)
                {
                    if (!AutoEncode(s, item))
                        return false;
                }
                return true;
            }
            return false;
        }

        // 编码固定长度的数组
        template<typename S, typename T>
        bool EncodeArray(S& s, const T* arr, size_t len)
        {
            uint16_t size = static_cast<uint16_t>(len);
            for (uint16_t i = 0; i < size; i++)
            {
                if (!AutoEncode(arr[i]))
                    return false;
            }
            return true;
        }

        // 对基本类型数组的特例化 直接内存拷贝 提高效率
        template<typename S>
        bool EncodeArray(S& s, const char* arr, size_t len)
        {
            return s.Write((const void*)arr, len*sizeof(char));
        }

        template<typename S>
        bool EncodeArray(S& s, const int* arr, size_t len)
        {
            return s.Write((const void*)arr, len*sizeof(int));
        }

        template<typename S>
        bool EncodeArray(S& s, const float* arr, size_t len)
        {
            return s.Write((const void*)arr, len*sizeof(float));
        }

        template<typename S>
        bool EncodeArray(S& s, const double* arr, size_t len)
        {
            return s.Write((const void*)arr, len*sizeof(double));
        }

        template<typename S>
        bool EncodeArray(S& s, const int64_t* arr, size_t len)
        {
            return s.Write((const void*)arr, len*sizeof(int64_t));
        }
#pragma endregion

#pragma  region 解码 Decode

        // 默认解码 对于POD类型 直接内存拷贝
        template<typename S, typename T>
        bool Decode(S& s, T& t)
        {
            static_assert(std::is_trivial<T>::value, "is not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "is not standard_layout. need to customize");
            return s.Read((void*)&t, sizeof(t));
        }

        // 对标准库容器的解码
        template<typename S>
        bool Decode(S& s, std::string& t)
        {
            return s.Read(t);
        }

        template<typename S>
        bool Decode(S& s, std::wstring& t)
        {
            return s.Read(t);
        }

        template<typename S, typename T>
        bool Decode(S& s, std::vector<T>& t)
        {
            return DecodeArray(s, t);
        }

        template<typename S, typename T>
        bool Decode(S& s, std::list<T>& t)
        {
            return DecodeArray(s, t);
        }

        // 解码数组 动态长度
        template<typename S, typename T>
        bool DecodeArray(S& s, T& t)
        {
            uint16_t size;
            if (s.Read(size))
            {
                for (uint16_t i = 0; i < size; i++)
                {
                    T::value_type v;
                    if (!AutoDecode(s, v))
                        return false;
                    t.push_back(v);
                }
            }
            return true;
        }

        // 解码数组 固定长度
        template<typename S, typename T>
        bool DecodeArray(S& s, T* arr, size_t arraySize)
        {
            uint16_t size = static_cast<uint16_t>arraySize;
            for (uint16_t i = 0; i < size; i++)
            {
                if (!AutoDecode(s, *(arr + i)))
                    return false;
            }
            return true;
        }

        // 对基本类型数组的特例化 直接内存拷贝
        template<typename S>
        bool DecodeArray(S& s, int* arr, size_t arraySize)
        {
            return s.Read((void*)arr, arraySize*sizeof(int));
        }

        template<typename S>
        bool DecodeArray(S& s, float* arr, size_t arraySize)
        {
            return s.Read((void*)arr, arraySize*sizeof(float));
        }

        template<typename S>
        bool DecodeArray(S& s, double* arr, size_t arraySize)
        {
            return s.Read((void*)arr, arraySize*sizeof(double));
        }

        template<typename S>
        bool DecodeArray(S& s, int64_t* arr, size_t arraySize)
        {
            return s.Read((void*)arr, arraySize*sizeof(int64_t));
        }

#pragma endregion

#pragma region 获取序列化大小 GetMsgSize

        // 默认实现 对于POD类型
        template<typename S, typename T>
        uint32_t GetMsgSize(S& s, const T& t)
        {
            static_assert(std::is_trivial<T>::value, "is not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "is not standard layout. need to customize");
            return s.GetSize((const void*)&t, sizeof(t));
        }

        // 标准库容器
        template<typename S, typename T>
        uint32_t GetMsgSize(S& s, const std::vector<T>& t)
        {
            return GetArrayMsgSize(s, t);
        }

        template<typename S, typename T>
        uint32_t GetMsgSize(S& s, const std::list<T>& t)
        {
            return GetArrayMsgSize(s, t);
        }

        template<typename S>
        uint32_t GetMsgSize(S& s, const std::string& t)
        {
            return s.GetSize(t);
        }

        template<typename S>
        uint32_t GetMsgSize(S& s, const std::wstring& t)
        {
            return s.GetSize(t);
        }

        // 得到数组编码后的大小 动态长度
        template<typename S, typename T>
        uint32_t GetArrayMsgSize(S& s, const T& t)
        {
            uint32_t total = 0;
            uint16_t size = static_cast<uint16_t>(t.size());
            total += s.GetSize(size);
            for (auto& item : t)
            {
                total += AutoMsgSize(s, item);
            }
            return total;
        }

        // 得到数组编码后的大小  固定长度
        template<typename S, typename T>
        uint32_t GetArrayMsgSize(S& s, const T* arr, size_t arraySize)
        {
            uint32_t total = 0;
            for (size_t i = 0; i < arraySize; i++)
            {
                total += AutoMsgSize(s, *(arr + i));
            }
            return total;
        }

        // 对基本类型的数组的特例化 
        template<typename S>
        uint32_t GetArrayMsgSize(S& s, const int* arr, size_t arraySize)
        {
            return s.GetSize((const void*)arr, arraySize*sizeof(int));
        }

        template<typename S>
        uint32_t GetArrayMsgSize(S& s, const float* arr, size_t arraySize)
        {
            return s.GetSize((const void*)arr, arraySize*sizeof(float));
        }

        template<typename S>
        uint32_t GetArrayMsgSize(S& s, const double* arr, size_t arraySize)
        {
            return s.GetSize((const void*)arr, arraySize*sizeof(double));
        }

        template<typename S>
        uint32_t GetArrayMsgSize(S& s, const int64_t* arr, size_t arraySize)
        {
            return s.GetSize((const void*)arr, arraySize*sizeof(int64_t));
        }

        // 全局获取对象编码大小
        template<typename T>
        uint32_t GetMsgSize(const T& t)
        {
            ProtocolSize sizer;
            return AutoMsgSize(sizer, t);
        }
#pragma endregion

#pragma region 序列化类 Serializer

        /*
        *   Serializer
        *   实际上是个模板推导分发函数 根据T是否是数组型别分发到不同的Serialize函数
        */
        template< bool IsArray >
        class Serializer
        {
        public:
            template<typename S, typename T>
            static bool Serialize(S& s, const T& t)
            {
                return Encode(s, t);
            }

            template<typename S, typename T>
            static bool DeSerialize(S& s,  T& t)
            {
                return Decode(s, t);
            }

            template<typename S, typename T>
            static uint32_t GetSize(S& s, const T& t)
            {
                return GetMsgSize(s, t);
            }
        };

        // Serializer 特例化  对于数组型别的对象序列化
        template<>
        class Serializer < true >
        {
        public:
            template<typename S, typename T, size_t arraySize>
            static bool Serialize(S& s, const T(&t)[arraySize])
            {
                return EncodeArray(s, t, arraySize);
            }

            template<typename S, typename T, size_t arraySize>
            static bool DeSerialize(S& s, const T(&t)[arraySize])
            {
                return DecodeArray(s, t, arraySize);
            }

            template<typename S, typename T, size_t arraySize>
            static bool GetSize(S& s, const T(&t)[arraySize])
            {
                return GetArrayMsgSize(s, t, arraySize);
            }
        };
#pragma endregion

#pragma  region 自动化全局模板推导

        template<typename S, typename T>
        bool AutoEncode(S& s, const T& t)
        {
            return Serializer<std::is_array<T>::value>::Serialize(s, t);
        }

        template<typename S, typename T>
        bool AutoDecode(S& s, T& t)
        {
            return Serializer<std::is_array<T>::value>::DeSerialize(s, t);
        }

        template<typename S, typename T>
        uint32_t AutoMsgSize(S& s, const T& t)
        {
            return Serializer<std::is_array<T>::value>::GetSize(s, t);
        }

#pragma  endregion

}
}

#endif