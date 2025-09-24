#ifndef __DUMMY_LOCK_H__
#define __DUMMY_LOCK_H__

#include <mutex>

class DummyLock {
private:
#ifdef USE_ATOMIC_LOCK
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
#else
    std::mutex mtx;
#endif
public:
    void lock() {
#ifdef USE_ATOMIC_LOCK
        while (flag.test_and_set(std::memory_order_acquire)) {
            // Spin wait
        }
#else
        mtx.lock();
#endif
    }

    void unlock() {
#ifdef USE_ATOMIC_LOCK
        flag.clear(std::memory_order_release);
#else
        mtx.unlock();
#endif
    }
};

#endif