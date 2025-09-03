#ifndef __NO_PREALLOC_SHARED_PTR_POOL_H__
#define __NO_PREALLOC_SHARED_PTR_POOL_H__
#include <memory>
#include <mutex>
#include <vector>

//#define USE_ATOMIC_LOCK

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
    SpinLock m_spinlock_ptr;
    SpinLock m_spinlock_available_ptrs;

public:
    AtomicSharedPtrPool() {
    }

    template<typename... Args>
    std::shared_ptr<T> getOutput(Args&&... args) {
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
        return new_ptr;
    }

    void setOutput(std::shared_ptr<T> new_ptr) {
        // Acquire spinlock for ptr
        m_spinlock_ptr.lock();

        m_ptr = new_ptr;
        m_spinlock_ptr.unlock();
    }

    std::shared_ptr<T> getInput(void) {
        // Acquire spinlock for ptr
        m_spinlock_ptr.lock();

        auto input = m_ptr;
        m_spinlock_ptr.unlock();
        return input;
    }

private:
    void recycle(T* ptr) {
        // Acquire spinlock for available_ptrs
        m_spinlock_available_ptrs.lock();
        //printf("Recycling pointer: %p\n", ptr);
        m_available_ptrs.push_back(ptr);
        m_spinlock_available_ptrs.unlock();
    }

    std::vector<T*> m_available_ptrs;
};

#endif