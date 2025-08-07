#include <stdio.h>
#include <memory>
#include <atomic>
#include <thread>
#include <random>
#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include <cassert>

#include "atomic_intrusive_ptr_pool.h"

class MyData {
public:
    int value;
    std::atomic<int> refcount{0}; //needed for intrusive_ptr
};

const size_t NUM_OF_CONSUMERS = 128;

int main(void)
{
    //allocation externally
    std::vector<MyData*> initial_ptrs;
    for (size_t i = 0; i < NUM_OF_CONSUMERS + 2; ++i) {
        initial_ptrs.push_back(new MyData());
    }
    AtomicIntrusivePtrPool<MyData> latest_data(initial_ptrs);

    std::vector<std::thread> consumers;
    //create NUM_OF_CONSUMERS, with the sleep time in the range of 10 ~ 100mS randomly
    for (size_t i = 0; i < NUM_OF_CONSUMERS; ++i) {
        consumers.emplace_back([i, &latest_data] {
            auto sleep_time_ms = rand() % 90 + 10; // Random sleep time between 10 and 100 ms
            while (true) {
                auto data = latest_data.getInput();
                if (data) {
                    printf("Consumer %zu: %d\n", i, data->value);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
            }
        });
    }

    //main thread is periodically updating the latest data
    while(true) {
        auto new_data = latest_data.getOutput();
        new_data->value = rand() % 100;
        printf("Main thread: %d\n", new_data->value);
        latest_data.setOutput(new_data);
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return 0;
}

//////////////////////For system heap monitor///////////////////
////////////////////////////////////////////////////////////////
void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    std::cout << "[new] size=" << size << " -> " << ptr << std::endl;
    return ptr;
}

void operator delete(void* ptr) noexcept {
    std::cout << "[delete] ptr=" << ptr << std::endl;
    std::free(ptr);
}

void* operator new[](std::size_t size) {
    void* ptr = std::malloc(size);
    std::cout << "[new[]] size=" << size << " -> " << ptr << std::endl;
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    std::cout << "[delete[]] ptr=" << ptr << std::endl;
    std::free(ptr);
}