#ifndef __NGSERVER_LOCKER_H_INCLUDE__
#define __NGSERVER_LOCKER_H_INCLUDE__

#include<atomic>

struct Locker
{
private:
    Locker(const Locker&) = delete;
    Locker& operator = (const Locker&) = delete;

public:
    Locker()
    {
        _lock.clear(std::memory_order_relaxed);
    }

    inline void  Lock()
    {
        while (_lock.test_and_set(std::memory_order_acquire));
    }

    inline void UnLock()
    {
        _lock.clear(std::memory_order_release);
    }

    inline bool TryLock()
    {
        return (!_lock.test_and_set(std::memory_order_acquire));
    }

    inline bool TryLock(int n)
    {
        for (int i = 0; i < n; ++i)
            if (!_lock.test_and_set(std::memory_order_acquire))
                return true;
        return false;
    }

private:
    std::atomic_flag _lock;
};

class AutoLocker
{
public:
    AutoLocker(Locker* lock):
        _locker(lock)
    {
        _locker->Lock();
    }

    ~AutoLocker()
    {
        _locker->UnLock();
    }
private:
    Locker* _locker;
};

#endif