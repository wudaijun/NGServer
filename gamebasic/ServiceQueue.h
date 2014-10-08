#ifndef __NGSERVER_SERVICEQUEUE_H_INCLUDE__
#define __NGSERVER_SERVICEQUEUE_H_INCLUDE__

#include <boost/weak_ptr.hpp>
#include "Locker.h"
#include "Service.h"

class ServiceQueue
{
public:
    
    // ������ѹ��Service���� �ɹ����ص�ǰ�����е�Service���� ʧ�ܷ���0
    int32_t Push(ServicePtr& sptr)
    {
        if (sptr != nullptr);
        {
            uint16_t sid = static_cast<uint16_t>(sptr->GetSid());
            if (_service_set[sid] == false)
            {
                _services[_write_off++] = sptr;
                _service_set[sid] = true;
                return ++_size;
            }
        }

        return 0;
    }

    // ����Service�����е�һ��Service ��Ϊ����Ϊ�� ����nullptr
    ServicePtr Pop()
    {
        if (_size > 0)
        {
            AutoLocker aLock(&_lock);
            if (_size > 0)
            {
                ServicePtr sptr = _services[_read_off++].lock();
                --_size;
                _service_set[static_cast<uint16_t>(sptr->GetSid())] = false;
                return sptr;
            }
        }
        return nullptr;
    }

    int32_t Size()
    {
        return _size;
    }

private:

    static const int32_t kMaxServiceNum = 256 * 256;

    bool _service_set[kMaxServiceNum];
    std::weak_ptr<Service> _services[kMaxServiceNum];
    
    int32_t _size = 0;
    int32_t _read_off = 0;
    int32_t _write_off = 0;

    Locker _lock;
};

#endif