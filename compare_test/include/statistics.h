#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <atomic>
class Statistics {
public:
    static Statistics& get_instance() {
        static Statistics instance;
        return instance;
    }

    void add_send_bytes(uint64_t bytes) {
        total_send_bytes.fetch_add(bytes, std::memory_order_relaxed);
    }

    void add_recv_bytes(uint64_t bytes) {
        total_recv_bytes.fetch_add(bytes, std::memory_order_relaxed);
    }

    void add_send_count(uint64_t count) {
        total_send_count.fetch_add(count, std::memory_order_relaxed);
    }

    void add_recv_count(uint64_t count) {
        total_recv_count.fetch_add(count, std::memory_order_relaxed);
    }

    void add_cpu_time_us(uint64_t us) {
        total_cpu_time_us.fetch_add(us, std::memory_order_relaxed);
    }

    uint64_t reset_send_bytes() {
        return total_send_bytes.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_recv_bytes() {
        return total_recv_bytes.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_send_count() {
        return total_send_count.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_recv_count() {
        return total_recv_count.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_cpu_time_us() {
        return total_cpu_time_us.exchange(0, std::memory_order_relaxed);
    }

    uint64_t add_system_heap_alloc_bytes(uint64_t bytes) {
        return system_heap_alloc_bytes.fetch_add(bytes, std::memory_order_relaxed);
    }

    uint64_t add_system_heap_free_bytes(uint64_t bytes) {
        return system_heap_free_bytes.fetch_add(bytes, std::memory_order_relaxed);
    }

    uint64_t add_system_heap_alloc_count(uint64_t count) {
        return system_heap_alloc_count.fetch_add(count, std::memory_order_relaxed);
    }

    uint64_t add_system_heap_free_count(uint64_t count) {
        return system_heap_free_count.fetch_add(count, std::memory_order_relaxed);
    }

    uint64_t reset_system_heap_alloc_bytes() {
        return system_heap_alloc_bytes.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_system_heap_free_bytes() {
        return system_heap_free_bytes.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_system_heap_alloc_count() {
        return system_heap_alloc_count.exchange(0, std::memory_order_relaxed);
    }

    uint64_t reset_system_heap_free_count() {
        return system_heap_free_count.exchange(0, std::memory_order_relaxed);
    }
private:
    Statistics() = default;
    std::atomic<uint64_t> total_send_bytes;
    std::atomic<uint64_t> total_recv_bytes;
    std::atomic<uint64_t> total_send_count;
    std::atomic<uint64_t> total_recv_count;
    std::atomic<uint64_t> total_cpu_time_us;
    std::atomic<uint64_t> system_heap_alloc_bytes;
    std::atomic<uint64_t> system_heap_free_bytes;
    std::atomic<uint64_t> system_heap_alloc_count;
    std::atomic<uint64_t> system_heap_free_count;
};
#endif
