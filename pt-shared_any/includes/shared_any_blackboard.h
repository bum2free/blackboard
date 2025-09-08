#ifndef __ANY_SHARED_BLACKBOARD_H__
#define __ANY_SHARED_BLACKBOARD_H__

#include <any>
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "shared_ptr_pool.h"
#include "common/dummy_lock.h"

class BlackBoardSharedAny {
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> getOutput(const char* topic, Args&&... args) {
        m_lock.lock();
        auto it = m_data_map.find(topic);
        if (it == m_data_map.end()) {
            it = m_data_map.emplace(topic, new SharedPtrPool<T>()).first;
        }
        m_lock.unlock();
        auto ptr = std::any_cast<SharedPtrPool<T>*>(it->second);
        if (ptr) {
            return ptr->getOutput(std::forward<Args>(args)...);
        }
        return nullptr;
    }

    template<typename T>
    void setOutput(const char* topic, const std::shared_ptr<T> &new_ptr) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            auto ptr = std::any_cast<SharedPtrPool<T>*>(it->second);
            if (ptr) {
                ptr->setOutput(new_ptr);
            }
        }
    }

    template<typename T>
    std::shared_ptr<T> getInput(const char* topic) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            auto ptr = std::any_cast<SharedPtrPool<T>*>(it->second);
            if (ptr) {
                return ptr->getInput();
            }
        }
        return nullptr;
    }

private:
    std::map<std::string, std::any> m_data_map;
    DummyLock m_lock;
};
#endif