#include <iostream>
#include <thread>
#include <stdio.h>
#include <vector>

#include "shared_any_blackboard.h"

class MyData1 {
public:
    int value;
};

class MyData2 {
public:
    float value;
};

BlackBoardSharedAny g_blackboard;

const size_t NUM_OF_CONSUMERS = 2;
int main(void)
{
    std::vector<std::thread> consumers;
    //create NUM_OF_CONSUMERS, with the sleep time in the range of 10 ~ 100mS randomly
    for (size_t i = 0; i < NUM_OF_CONSUMERS; ++i) {
        consumers.emplace_back([i] {
            auto sleep_time_ms = rand() % 90 + 10; // Random sleep time between 10 and 100 ms
            while (true) {
                auto data1 = g_blackboard.getInput<MyData1>("data1");
                if (data1) {
                    printf("Consumer %zu: data1: %d\n", i, data1->value);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
                
                auto data2 = g_blackboard.getInput<MyData2>("data2");
                if (data2) {
                    printf("Consumer %zu: data2: %f\n", i, data2->value);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
            }
        });
    }

     //main thread is periodically updating the latest data
    while(true) {
        auto new_data_1 = g_blackboard.getOutput<MyData1>("data1");
        new_data_1->value = rand() % 100;
        printf("Main thread: data1: %d\n", new_data_1->value);
        g_blackboard.setOutput("data1", new_data_1);
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        auto new_data_2 = g_blackboard.getOutput<MyData2>("data2");
        new_data_2->value = static_cast<float>(rand()) / RAND_MAX;
        printf("Main thread: data2: %f\n", new_data_2->value);
        g_blackboard.setOutput("data2", new_data_2);
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