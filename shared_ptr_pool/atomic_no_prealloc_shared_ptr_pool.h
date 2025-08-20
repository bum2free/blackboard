#ifndef __ATOMIC_NO_PREALLOC_SHARED_PTR_POOL_H__
#define __ATOMIC_NO_PREALLOC_SHARED_PTR_POOL_H__
#include <memory>
#include <set>
#include <vector>

template<typename T>
class AtomicSharedPtrPool {
    class SpinLock {
    private:
        std::atomic_flag flag = ATOMIC_FLAG_INIT;
    public:
        void lock() {
            while (flag.test_and_set(std::memory_order_acquire)) {
                // Spin wait
            }
        }

        void unlock() {
            flag.clear(std::memory_order_release);
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
        bool has_available = !m_available_ptrs.empty();

        if (!has_available) {
            m_spinlock_available_ptrs.unlock();
            auto *raw = new T(std::forward<Args>(args)...);
            m_spinlock_available_ptrs.lock();
            m_available_ptrs.insert(raw);
            printf("Created new pointer: %p\n", raw);
        }
        //get a pointer from the available set
        auto it = m_available_ptrs.begin();
        std::shared_ptr<T> new_ptr(*it, [this](T* ptr) {
            recycle(ptr);
        });
        m_available_ptrs.erase(it);

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
        printf("Recycling pointer: %p\n", ptr);
        m_available_ptrs.insert(ptr);

        m_spinlock_available_ptrs.unlock();
    }

    std::set<T*> m_available_ptrs;
};

#endif