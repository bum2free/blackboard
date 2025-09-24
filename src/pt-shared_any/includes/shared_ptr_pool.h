#ifndef __SHARED_PTR_POOL_H__
#define __SHARED_PTR_POOL_H__
#include <memory>
#include <vector>

#ifdef USE_CONCURRENTQUEUE
#include "concurrentqueue/concurrentqueue.h"
#else
#include "dummy_lock.h"
#endif

template<typename T>
class SharedPtrPool {
private:
    std::shared_ptr<T> m_ptr = nullptr;
#ifndef USE_CONCURRENTQUEUE
    DummyLock m_lock_available_ptrs;
#endif

public:
    SharedPtrPool() {
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
        // Acquire lock for available_ptrs
        m_lock_available_ptrs.lock();
        if (m_available_ptrs.empty()) {
            // Allocate outside the lock
            m_lock_available_ptrs.unlock();
            T* raw = new T(std::forward<Args>(args)...);
            m_lock_available_ptrs.lock();
            m_available_ptrs.push_back(raw);
            //printf("Created new pointer: %p\n", raw);
        }
        // LIFO reuse for better cache locality
        T* ptr = m_available_ptrs.back();
        m_available_ptrs.pop_back();
        std::shared_ptr<T> new_ptr(ptr, [this](T* p) { recycle(p); });
        m_lock_available_ptrs.unlock();
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
        // Acquire lock for available_ptrs
        m_lock_available_ptrs.lock();
        //printf("Recycling pointer: %p\n", ptr);
        m_available_ptrs.push_back(ptr);
        m_lock_available_ptrs.unlock();
#endif
    }
#ifdef USE_CONCURRENTQUEUE
    moodycamel::ConcurrentQueue<T*> m_available_ptrs;
#else
    std::vector<T*> m_available_ptrs;
#endif
};

#endif
