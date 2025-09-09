#ifndef __ANY_SHARED_CYBER_RECORDER_BLACKBOARD_H__
#define __ANY_SHARED_CYBER_RECORDER_BLACKBOARD_H__

#include <any>
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "shared_ptr_pool.h"
#include "common/dummy_lock.h"

#include "cyber/cyber.h"

class BlackBoardSharedAnyCyberRecorder {
public:
    enum class Mode {
        NORMAL,
        RECORD,
        PLAYBACK
    };

    BlackBoardSharedAnyCyberRecorder(Mode mode) {
        m_mode = mode;
        apollo::cyber::Init("shared_any_cyber_recorder_blackboard");
        m_cyber_node = apollo::cyber::CreateNode("shared_any_cyber_recorder_blackboard_node");
    }

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
    bool setOutput(const char* topic, const std::shared_ptr<T> &new_ptr, bool from_recorder = false) {
        bool sent = false;
        SharedPtrPool<T>* poolPtr = nullptr;
        std::shared_ptr<apollo::cyber::Writer<T>> cyber_writer = nullptr;

        m_lock.lock();
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            poolPtr = std::any_cast<SharedPtrPool<T>*>(it->second);
        }
        if (m_mode == Mode::RECORD) {
            if (auto it = m_cyber_writers.find(topic); it != m_cyber_writers.end()) {
                cyber_writer = std::dynamic_pointer_cast<apollo::cyber::Writer<T>>(it->second);
            } else {
                cyber_writer = m_cyber_node->CreateWriter<T>(topic);
                if (cyber_writer) {
                    m_cyber_writers.emplace(topic, cyber_writer);
                }
            }
        }
        m_lock.unlock();

        if (poolPtr && (m_mode != Mode::PLAYBACK || (m_mode == Mode::PLAYBACK && from_recorder))) {
            poolPtr->setOutput(new_ptr);
            sent = true;
        }
        if (m_mode == Mode::RECORD && cyber_writer && new_ptr) {
            cyber_writer->Write(new_ptr);
        }

        return sent;
    }

    template<typename T>
    std::shared_ptr<T> getInput(const char* topic) {
        SharedPtrPool<T>* poolPtr = nullptr;

        m_lock.lock();
        if (auto it = m_data_map.find(topic); it != m_data_map.end()) {
            poolPtr = std::any_cast<SharedPtrPool<T>*>(it->second);
        }
        if (m_mode == Mode::PLAYBACK) {
            if (auto it = m_cyber_readers.find(topic); it == m_cyber_readers.end()) {
                std::string topic_str(topic);
                auto cyber_reader = m_cyber_node->CreateReader<T>(topic, [this, topic_str](const std::shared_ptr<T>& msg) {
                    this->setOutput<T>(topic_str.c_str(), msg, true);
                });
                if (cyber_reader) {
                    m_cyber_readers.emplace(topic, cyber_reader);
                }
            }
        }
        m_lock.unlock();

        if (poolPtr) {
            return poolPtr->getInput();
        }
        return nullptr;
    }

private:
    Mode m_mode;
    std::map<std::string, std::any> m_data_map;
    std::map<std::string, std::shared_ptr<apollo::cyber::WriterBase>> m_cyber_writers;
    std::map<std::string, std::shared_ptr<apollo::cyber::ReaderBase>> m_cyber_readers;
    std::unique_ptr<apollo::cyber::Node> m_cyber_node;
    DummyLock m_lock;
};
#endif
