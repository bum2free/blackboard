#ifndef __VARIANT_BLACKBOARD_1_H__
#define __VARIANT_BLACKBOARD_1_H__
#include <map>
#include <memory>
#include <mutex>
#include <variant>
#include <shared_mutex>

#include "intrusive_ptr_pool.h"

template<typename... DataTypes>
class BlackBoardIntrusiveVariant {
public:
    template<typename T, typename... Args>
    boost::intrusive_ptr<IntrusiveWrapper<T>> getOutput(const char* topic, Args&&... args) {
        // hold read lock
        std::shared_lock<std::shared_mutex> read_lock(m_mutex);
        auto it = m_data_map.find(topic);
        if (it == m_data_map.end()) {
            // release read lock
            read_lock.unlock();
            // hold write lock
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            // Check again in case another thread inserted while we were waiting for write lock
            it = m_data_map.find(topic);
            if (it == m_data_map.end()) {
                m_data_map.emplace(topic, std::make_unique<IntrusivePtrPool<T>>());
                it = m_data_map.find(topic);
            }
            // release write lock, acquire read lock
            write_lock.unlock();
            read_lock.lock();
        }
        if (auto ptr = std::get_if<std::unique_ptr<IntrusivePtrPool<T>>>(&(it->second))) {
            return (*ptr)->getOutput(std::forward<Args>(args)...);
        }
        return nullptr;
    }

    template<typename T>
    void setOutput(const char* topic, const boost::intrusive_ptr<IntrusiveWrapper<T>> &new_ptr) {
        std::shared_lock<std::shared_mutex> read_lock(m_mutex);
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            if (auto ptr = std::get_if<std::unique_ptr<IntrusivePtrPool<T>>>(&(it->second))) {
                (*ptr)->setOutput(new_ptr);
            }
        }
    }

    template<typename T>
    boost::intrusive_ptr<IntrusiveWrapper<T>> getInput(const char* topic) {
        std::shared_lock<std::shared_mutex> read_lock(m_mutex);
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            if (auto ptr = std::get_if<std::unique_ptr<IntrusivePtrPool<T>>>(&(it->second))) {
                return (*ptr)->getInput();
            }
        }
        return nullptr;
    }

private:
    using IntrusivePtrPoolVariant = std::variant<
        std::unique_ptr<IntrusivePtrPool<DataTypes>>...
        >;
    std::map<std::string, IntrusivePtrPoolVariant> m_data_map;
    mutable std::shared_mutex m_mutex;
};

#endif