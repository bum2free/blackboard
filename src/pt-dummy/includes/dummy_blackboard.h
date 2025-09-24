#ifndef __DUMMY_BLACKBOARD_H__
#define __DUMMY_BLACKBOARD_H__

#include <any>
#include <atomic>
#include <optional>
#include <string>
#include <map>
#include <mutex>

//#define USE_ATOMIC_LOCK

#ifdef USE_ATOMIC_LOCK
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
#endif

class BlackboardDummy {
public:
    static BlackboardDummy& get_instance() {
        static BlackboardDummy instance;
        return instance;
    }
    template<typename T>
    void save(const std::string& key, const T& value) {
        //std::lock_guard<std::mutex> lock(mtx);
#ifdef USE_ATOMIC_LOCK
        spin_lock.lock();
#else
        std::lock_guard<std::mutex> lock(mtx);
#endif
        data_store[key] = value;
#ifdef USE_ATOMIC_LOCK
        spin_lock.unlock();
#endif
    }

    template<typename T>
    std::optional<T> load(const std::string& key) {
        //std::lock_guard<std::mutex> lock(mtx);
#ifdef USE_ATOMIC_LOCK
        spin_lock.lock();
#else
        std::lock_guard<std::mutex> lock(mtx);
#endif
        auto it = data_store.find(key);
        if (it != data_store.end()) {
            try {
#ifdef USE_ATOMIC_LOCK
                spin_lock.unlock();
#endif
                return std::optional<T>(std::any_cast<T>(it->second));
            } catch (const std::bad_any_cast&) {
#ifdef USE_ATOMIC_LOCK
                spin_lock.unlock();
#endif
                return std::nullopt; // Type mismatch
            }
        }
#ifdef USE_ATOMIC_LOCK
        spin_lock.unlock();
#endif
        return std::nullopt; // Key not found
    }
private:
    BlackboardDummy() = default;
    std::map<std::string, std::any> data_store;
    std::mutex mtx;
#ifdef USE_ATOMIC_LOCK
    SpinLock spin_lock;
#endif
};
#endif
