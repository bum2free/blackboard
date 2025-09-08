#ifndef __VARIANT_BLACKBOARD_1_H__
#define __VARIANT_BLACKBOARD_1_H__
#include <map>
#include <memory>
#include <variant>

#include "atomic_intrusive_ptr_pool_1.h"

template<typename... DataTypes>
class BlackBoardVariant {
public:
    template<typename T>
    void register_topic(const char* topic, const std::vector<T*>& initial_ptrs) {
        m_data_map.emplace(topic, std::make_unique<AtomicIntrusivePtrPool<T>>(initial_ptrs));
    }

    template<typename T>
    boost::intrusive_ptr<T> getOutput(const char* topic) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            if (auto ptr = std::get_if<std::unique_ptr<AtomicIntrusivePtrPool<T>>>(&(it->second))) {
                return (*ptr)->getOutput();
            }
        }
        return nullptr;
    }

    template<typename T>
    void setOutput(const char* topic, const boost::intrusive_ptr<T> &new_ptr) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            if (auto ptr = std::get_if<std::unique_ptr<AtomicIntrusivePtrPool<T>>>(&(it->second))) {
                (*ptr)->setOutput(new_ptr);
            }
        }
    }

    template<typename T>
    boost::intrusive_ptr<T> getInput(const char* topic) {
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            if (auto ptr = std::get_if<std::unique_ptr<AtomicIntrusivePtrPool<T>>>(&(it->second))) {
                return (*ptr)->getInput();
            }
        }
        return nullptr;
    }

private:
    using AtomicIntrusivePtrPoolVariant = std::variant<
        std::unique_ptr<AtomicIntrusivePtrPool<DataTypes>>...
        >;
    std::map<std::string, AtomicIntrusivePtrPoolVariant> m_data_map;
};

#endif