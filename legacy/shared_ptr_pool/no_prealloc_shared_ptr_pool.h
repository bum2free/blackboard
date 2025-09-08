#ifndef __NO_PREALLOC_SHARED_PTR_POOL_H__
#define __NO_PREALLOC_SHARED_PTR_POOL_H__
#include <memory>
#include <mutex>
#include <vector>

//#define USE_ATOMIC_LOCK
#define USE_CONCURRENTQUEUE

#ifdef USE_CONCURRENTQUEUE
#include "../concurrentqueue/concurrentqueue.h"
#endif

template<typename T>
class AtomicSharedPtrPool {
    class SpinLock {
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

private:
    std::shared_ptr<T> m_ptr = nullptr;
#ifndef USE_CONCURRENTQUEUE
    SpinLock m_spinlock_available_ptrs;
#endif

public:
    AtomicSharedPtrPool() {
    }

    template<typename... Args>
    std::shared_ptr<T> getOutput(Args&&... args) {
#ifdef USE_CONCURRENTQUEUE
        T* raw_ptr;
        bool found = m_available_ptrs.try_dequeue(raw_ptr);
        if (!found) {
            raw_ptr = new T(std::forward<Args>(args)...);
        }
        std::shared_ptr<T> new_ptr(raw_ptr, [this](T* p) { recycle(p); });
        return new_ptr;
#else
        // Acquire spinlock for available_ptrs
        m_spinlock_available_ptrs.lock();
        if (m_available_ptrs.empty()) {
            // Allocate outside the lock
            m_spinlock_available_ptrs.unlock();
            T* raw = new T(std::forward<Args>(args)...);
            m_spinlock_available_ptrs.lock();
            m_available_ptrs.push_back(raw);
            //printf("Created new pointer: %p\n", raw);
        }
        // LIFO reuse for better cache locality
        T* ptr = m_available_ptrs.back();
        m_available_ptrs.pop_back();
        std::shared_ptr<T> new_ptr(ptr, [this](T* p) { recycle(p); });
        m_spinlock_available_ptrs.unlock();
#endif
        return new_ptr;
    }

    void setOutput(std::shared_ptr<T> new_ptr) {
        std::atomic_store(&m_ptr, new_ptr);
    }

    std::shared_ptr<T> getInput(void) {
        return std::atomic_load(&m_ptr);
    }

private:
    void recycle(T* ptr) {
#ifdef USE_CONCURRENTQUEUE
        m_available_ptrs.enqueue(ptr);
#else
        // Acquire spinlock for available_ptrs
        m_spinlock_available_ptrs.lock();
        //printf("Recycling pointer: %p\n", ptr);
        m_available_ptrs.push_back(ptr);
        m_spinlock_available_ptrs.unlock();
#endif
    }
#ifdef USE_CONCURRENTQUEUE
    moodycamel::ConcurrentQueue<T*> m_available_ptrs;
#else
    std::vector<T*> m_available_ptrs;
#endif
};

#endif