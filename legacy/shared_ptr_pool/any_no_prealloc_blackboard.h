#ifndef __ANY_NO_PREALLOC_BLACKBOARD_H__
#define __ANY_NO_PREALLOC_BLACKBOARD_H__

#include <any>
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "no_prealloc_shared_ptr_pool.h"

class BlackBoardAny {
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
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> getOutput(const char* topic, Args&&... args) {
        m_spinlock.lock();
        auto it = m_data_map.find(topic);
        if (it == m_data_map.end()) {
            it = m_data_map.emplace(topic, new AtomicSharedPtrPool<T>()).first;
        }
        m_spinlock.unlock();
        auto ptr = std::any_cast<AtomicSharedPtrPool<T>*>(it->second);
        if (ptr) {
            return ptr->getOutput(std::forward<Args>(args)...);
        }
        return nullptr;
    }

    template<typename T>
    void setOutput(const char* topic, const std::shared_ptr<T> &new_ptr) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            auto ptr = std::any_cast<AtomicSharedPtrPool<T>*>(it->second);
            if (ptr) {
                ptr->setOutput(new_ptr);
            }
        }
    }

    template<typename T>
    std::shared_ptr<T> getInput(const char* topic) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            auto ptr = std::any_cast<AtomicSharedPtrPool<T>*>(it->second);
            if (ptr) {
                return ptr->getInput();
            }
        }
        return nullptr;
    }

private:
    std::map<std::string, std::any> m_data_map;
    SpinLock m_spinlock;
};
#endif