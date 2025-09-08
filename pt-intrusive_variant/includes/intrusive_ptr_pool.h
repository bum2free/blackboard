#ifndef __ATOMIC_INTRUSIVE_PTR_POOL_H__
#define __ATOMIC_INTRUSIVE_PTR_POOL_H__

#include <boost/intrusive_ptr.hpp>
#include <iostream>
#include <vector>

#include "common/dummy_lock.h"

template<typename T>
class IntrusiveWrapper {
public:
    std::atomic<int> refcount{0};

    IntrusiveWrapper(T* ptr, void *poll) : p_data(ptr), pool(poll) {}

    T* get() { return p_data; }
public:
    T* p_data;
    void *pool = nullptr;
};

template<typename T>
class IntrusivePtrPool {
private:
    boost::intrusive_ptr<IntrusiveWrapper<T>> m_ptr = nullptr;
    DummyLock m_DummyLock;
    DummyLock m_DummyLock_available_ptrs;
    
public:
    IntrusivePtrPool() {}

    template<typename... Args>
    boost::intrusive_ptr<IntrusiveWrapper<T>> getOutput(Args&&... args) {
        m_DummyLock_available_ptrs.lock();
        if (m_map_ptrs.empty()) {
            m_DummyLock_available_ptrs.unlock();
            auto *raw = new T(std::forward<Args>(args)...);
            auto ptr = new IntrusiveWrapper<T>(raw, this);
            auto new_ptr = boost::intrusive_ptr<IntrusiveWrapper<T>>(ptr);
            return new_ptr;
        }
        auto new_ptr = m_map_ptrs.back();
        //remove it from the set
        m_map_ptrs.pop_back();
        m_DummyLock_available_ptrs.unlock();
        return new_ptr;
    }

    void setOutput(const boost::intrusive_ptr<IntrusiveWrapper<T>> &new_ptr) {
        m_DummyLock.lock();
        m_ptr = new_ptr;
        m_DummyLock.unlock();
    }

    boost::intrusive_ptr<IntrusiveWrapper<T>> getInput(void) {
        m_DummyLock.lock();
        auto result = m_ptr;
        m_DummyLock.unlock();
        return result;
    }

    void recycle(IntrusiveWrapper<T>* ptr) {
        //printf("Recycling pointer: %p\n", ptr);
        m_DummyLock_available_ptrs.lock();
        m_map_ptrs.push_back(boost::intrusive_ptr<IntrusiveWrapper<T>>(ptr));
        m_DummyLock_available_ptrs.unlock();
    }

    std::vector<boost::intrusive_ptr<IntrusiveWrapper<T>>> m_map_ptrs;
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
        IntrusivePtrPool<T>* pool = static_cast<IntrusivePtrPool<T>*>(p->pool);
        if (pool) {
            pool->recycle(p);
        } else {
            std::cerr << "Warning: IntrusivePtrPool is null, cannot recycle pointer: " << p << std::endl;
        }
    }
    //std::cout << "Releasing reference: " << p << ", new refcount: " << p->refcount << std::endl;
}

#endif
