#ifndef __ATOMIC_INTRUSIVE_PTR_POOL_H__
#define __ATOMIC_INTRUSIVE_PTR_POOL_H__

#include <boost/intrusive_ptr.hpp>
#include <vector>

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

template<typename T>
class IntrusiveWrapper {
public:
    std::atomic<int> refcount{0};

    IntrusiveWrapper(T* ptr) : p_data(ptr) {}

    T* get() { return p_data; }
private:
    T* p_data;
};

template<typename T>
class IntrusivePtrPool {
private:
    boost::intrusive_ptr<IntrusiveWrapper<T>> m_ptr = nullptr;
    SpinLock m_spinlock;
    SpinLock m_spinlock_available_ptrs;
    
public:
    IntrusivePtrPool() {}

    template<typename... Args>
    boost::intrusive_ptr<IntrusiveWrapper<T>> getOutput(Args&&... args) {
        m_spinlock_available_ptrs.lock();
        auto it = std::find_if(m_map_ptrs.begin(), m_map_ptrs.end(), [](const auto& pair) {
            return pair.first->refcount == 1;
        });
        if (it == m_map_ptrs.end()) {
            auto *raw = new T(std::forward<Args>(args)...);
            auto ptr = new IntrusiveWrapper<T>(raw);
            m_map_ptrs[ptr] = boost::intrusive_ptr<IntrusiveWrapper<T>>(ptr);
            m_spinlock_available_ptrs.unlock();
            return ptr;
        }
        auto new_ptr = m_map_ptrs[it->first];
        m_spinlock_available_ptrs.unlock();
        return new_ptr;
    }

    void setOutput(const boost::intrusive_ptr<IntrusiveWrapper<T>> &new_ptr) {
        m_spinlock.lock();
        m_ptr = new_ptr;
        m_spinlock.unlock();
    }

    boost::intrusive_ptr<IntrusiveWrapper<T>> getInput(void) {
        m_spinlock.lock();
        auto result = m_ptr;
        m_spinlock.unlock();
        return result;
    }

    void recycle(IntrusiveWrapper<T>* ptr) {
        //printf("Recycling pointer: %p\n", ptr);
    }

    std::map<IntrusiveWrapper<T>*, boost::intrusive_ptr<IntrusiveWrapper<T>>> m_map_ptrs;
};

// Required by boost::intrusive_ptr
template<typename T>
inline void intrusive_ptr_add_ref(IntrusiveWrapper<T>* p) {
    ++p->refcount;
    //std::cout << "Adding reference: " << p << ", new refcount: " << p->refcount << std::endl;
}
template<typename T>
inline void intrusive_ptr_release(IntrusiveWrapper<T>* p) {
    if (--p->refcount == 0) {
        std::cout << "Deleting intrusive_ptr: " << p << std::endl;
        delete p;
        return;
    }
    //std::cout << "Releasing reference: " << p << ", new refcount: " << p->refcount << std::endl;
}

#endif