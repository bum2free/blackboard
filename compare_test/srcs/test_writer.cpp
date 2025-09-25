#include "test_writer.h"
#include "cpu_load.h"
#include "statistics.h"

#include <cstring>
#include <memory>
#include <signal.h>
#include <thread>

#ifndef USE_THREAD_TIMER
static void __cb_posix_timer(union sigval sv) {
    auto* writer = static_cast<TestWriter*>(sv.sival_ptr);
    if (writer) {
        writer->run();
        // Restart the timer for the next interval
        writer->start();
    }
}
#endif

TestWriter::TestWriter(const TestWriterDescription &desc) {
    this->name = desc.name;
    this->interval_ms = desc.interval_ms;
    this->burden_ms = desc.burden_ms;

#ifndef USE_THREAD_TIMER
    //initialize timer
    {
        struct sigevent sev;
        memset(&sev, 0, sizeof(sev));
        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_notify_function = __cb_posix_timer;
        sev.sigev_value.sival_ptr = this;

        timer_create(CLOCK_MONOTONIC, &sev, &timer);
    }
#endif
}

void TestWriter::start(void) {
#ifdef USE_THREAD_TIMER
    timer_thread = std::thread([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            this->run();
        }
    });
    pthread_setname_np(timer_thread.native_handle(), "Writer Timer");
#else
    struct itimerspec its;
    its.it_value.tv_sec = interval_ms / 1000;
    its.it_value.tv_nsec = (interval_ms % 1000) * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    timer_settime(timer, 0, &its, nullptr);
#endif
}

void TestWriter::run(void) {
    auto& statistics = Statistics::get_instance();
    uint64_t cpu_us = consume_cpu_in_ms(burden_ms, [this, &statistics]() {
        auto result = run_send();
        statistics.add_send_count(result.first);
        statistics.add_send_bytes(result.second);
    });

    statistics.add_cpu_time_us(cpu_us);
}