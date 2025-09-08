#ifndef __ATOMIC_INTRUSIVE_PTR_POOL_1_H__
#define __ATOMIC_INTRUSIVE_PTR_POOL_1_H__

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
class AtomicIntrusivePtrPool {
private:
    boost::intrusive_ptr<T> m_ptr = nullptr;
    SpinLock m_spinlock;
    
public:
    AtomicIntrusivePtrPool(const std::vector<T*>& initial_ptrs) {
        for (auto ptr : initial_ptrs) {
            printf("Created new intrusive_ptr: %p\n", ptr);
            m_map_ptrs[ptr] = boost::intrusive_ptr<T>(ptr);
        }
    }
    
    boost::intrusive_ptr<T> getOutput(void) {
        auto it = std::find_if(m_map_ptrs.begin(), m_map_ptrs.end(), [](const auto& pair) {
            return pair.first->refcount == 1;
        });
        //assert if it is not found
        assert(it != m_map_ptrs.end());
        auto new_ptr = m_map_ptrs[it->first];
        return new_ptr;
    }

    void setOutput(const boost::intrusive_ptr<T> &new_ptr) {
        m_spinlock.lock();
        m_ptr = new_ptr;
        m_spinlock.unlock();
    }

    boost::intrusive_ptr<T> getInput(void) {
        m_spinlock.lock();
        auto result = m_ptr;
        m_spinlock.unlock();
        return result;
    }

    void recycle(T* ptr) {
        printf("Recycling pointer: %p\n", ptr);
    }

    std::map<T*, boost::intrusive_ptr<T>> m_map_ptrs;
};

// Required by boost::intrusive_ptr
template<typename T>
inline void intrusive_ptr_add_ref(T* p) {
    ++p->refcount;
    //std::cout << "Adding reference: " << p << ", new refcount: " << p->refcount << std::endl;
}
template<typename T>
inline void intrusive_ptr_release(T* p) {
    --p->refcount;
    //std::cout << "Releasing reference: " << p << ", new refcount: " << p->refcount << std::endl;
}

#endif