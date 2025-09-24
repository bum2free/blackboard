#ifndef __CPU_LOAD_H__
#define __CPU_LOAD_H__

#include <functional>
#include <stdint.h>

// Function to consume CPU time for a specified duration
// Description:
// This function is designed to consume CPU resources for a specified duration.
// It first calls an optional callback function, if provided; if the callback
// function consumes less time than the specified duration, it will then
// busy-wait until the specified duration has elapsed.
//
// Input:
// - duration in milliseconds
// - external one_time callback function to be called once if exist
// Output:
// - actual duration in microseconds consumed
uint64_t consume_cpu_in_ms(uint32_t duration_ms,
    std::function<void()> one_time_callback = nullptr);
uint64_t consume_cpu_in_us(uint32_t duration_ms,
    std::function<void()> one_time_callback = nullptr);
#endif
