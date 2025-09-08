#ifndef __ANY_BLACKBOARD_H__
#define __ANY_BLACKBOARD_H__

#include <any>
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "atomic_shared_ptr_pool.h"

class BlackBoardAny {
public:
    template<typename T>
    void register_topic(const char* topic, const std::vector<T*>& raw_ptrs) {
        m_data_map.emplace(topic, new AtomicSharedPtrPool<T>(raw_ptrs));
    }

    template<typename T>
    std::shared_ptr<T> getOutput(const char* topic) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            auto ptr = std::any_cast<AtomicSharedPtrPool<T>*>(it->second);
            if (ptr) {
                return ptr->getOutput();
            }
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
};
#endif