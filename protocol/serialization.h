#ifndef __NGSERVER_SERIALIZATION_H_INCLUDE__
#define __NGSERVER_SERIALIZATION_H_INCLUDE__

/*
*   serialization.h  Created : 2014-10-14 afternoon
*   �����ȫ��ģ���Ƶ� ��ProtocolReader ProtocolWriter�е���
*   ���ļ�ͨ�������ݵ�ģ���Ƶ����ͱ�ʶ�� ���ս����ӵ��������ͷֽ�Ϊ�򵥵���������
*   ����ͨ�� ProtocolReader / ProtocolWriter �� Read / Write ��ִ��д��Ͷ���
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


#pragma region ���� Encode

        // Ĭ�ϱ��뺯��  ������û��Զ����POD����
        template<typename S, typename T>
        bool Encode(S& s, const T& t)
        {
            static_assert(std::is_trivial<T>::value, "not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "not standard layout. need to customize");
            return s.Write((const void*)&t, sizeof(t));
        }

        // �Լ�������������
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

        // ���붯̬���ȵ����� ( list . vector)
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

        // ����̶����ȵ�����
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

        // �Ի������������������ ֱ���ڴ濽�� ���Ч��
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

#pragma  region ���� Decode

        // Ĭ�Ͻ��� ����POD���� ֱ���ڴ濽��
        template<typename S, typename T>
        bool Decode(S& s, T& t)
        {
            static_assert(std::is_trivial<T>::value, "is not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "is not standard_layout. need to customize");
            return s.Read((void*)&t, sizeof(t));
        }

        // �Ա�׼�������Ľ���
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

        // �������� ��̬����
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

        // �������� �̶�����
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

        // �Ի������������������ ֱ���ڴ濽��
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

#pragma region ��ȡ���л���С GetMsgSize

        // Ĭ��ʵ�� ����POD����
        template<typename S, typename T>
        uint32_t GetMsgSize(S& s, const T& t)
        {
            static_assert(std::is_trivial<T>::value, "is not trivial. need to customize");
            static_assert(std::is_standard_layout<T>::value, "is not standard layout. need to customize");
            return s.GetSize((const void*)&t, sizeof(t));
        }

        // ��׼������
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

        // �õ���������Ĵ�С ��̬����
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

        // �õ���������Ĵ�С  �̶�����
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

        // �Ի������͵������������ 
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

        // ȫ�ֻ�ȡ��������С
        template<typename T>
        uint32_t GetMsgSize(const T& t)
        {
            ProtocolSize sizer;
            return AutoMsgSize(sizer, t);
        }
#pragma endregion

#pragma region ���л��� Serializer

        /*
        *   Serializer
        *   ʵ�����Ǹ�ģ���Ƶ��ַ����� ����T�Ƿ��������ͱ�ַ�����ͬ��Serialize����
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

        // Serializer ������  ���������ͱ�Ķ������л�
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

#pragma  region �Զ���ȫ��ģ���Ƶ�

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