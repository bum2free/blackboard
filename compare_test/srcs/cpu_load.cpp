#include "auto_test/cpu_load.h"

#include <chrono>
uint64_t consume_cpu_in_ms(uint32_t duration_ms,
    std::function<void()> one_time_callback)
{
    return consume_cpu_in_us(duration_ms * 1000, one_time_callback);
}

uint64_t consume_cpu_in_us(uint32_t duration_us,
    std::function<void()> one_time_callback)
{
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    if (one_time_callback) {
        one_time_callback();
    }
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    uint64_t elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    if (elapsed_time < duration_us) {
        // Busy-wait until the specified duration has elapsed
        while (elapsed_time < duration_us) {
            end_time = std::chrono::steady_clock::now();
            //TBD: add unit cpu consume handler
            elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        }
    }
    return elapsed_time; // Return the actual duration in microseconds consumed
}
