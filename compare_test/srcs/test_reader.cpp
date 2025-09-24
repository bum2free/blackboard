#include "test_reader.h"
#include "cpu_load.h"
#include "statistics.h"

#include "dummy_blackboard.h"
#include "shared_any_blackboard.h"
#include "intrusive_variant_blackboard.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <signal.h>
#include <thread>

static void __cb_posix_timer(union sigval sv) {
    auto* reader = static_cast<TestReader*>(sv.sival_ptr);
    if (reader) {
        reader->run();
        // Restart the timer for the next interval
        reader->start();
    }
}

TestReader::TestReader(const TestReaderDescription& desc)
    : name(desc.name), interval_ms(desc.interval_ms), burden_ms(desc.burden_ms)
{
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

void TestReader::start(void) {
#ifdef USE_THREAD_TIMER
    timer_thread = std::thread([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
            this->run();
        }
    });
    pthread_setname_np(timer_thread.native_handle(), "Reader Timer");
#else
    struct itimerspec its;
    its.it_value.tv_sec = interval_ms / 1000;
    its.it_value.tv_nsec = (interval_ms % 1000) * 1000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    timer_settime(timer, 0, &its, nullptr);
#endif
}

void TestReader::run(void) {
   auto& statistics = Statistics::get_instance();
   uint64_t cpu_us = consume_cpu_in_ms(burden_ms, [this, &statistics]() {
       auto result = run_receive();
       statistics.add_recv_count(result.first);
       statistics.add_recv_bytes(result.second);
   });
   statistics.add_cpu_time_us(cpu_us);
}

//////////////////////////////////////////////////////////////////////////////
TestReaderDummy::TestReaderDummy(const TestReaderDescription& desc)
    : TestReader(desc) {
    this->payload_descs = desc.payload_descs;
}

std::pair<size_t, size_t> TestReaderDummy::run_receive(void) {
    size_t recv_count = 0, recv_bytes = 0;
    auto& blackboard = BlackboardDummy::get_instance();
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            auto payload = blackboard.load<std::shared_ptr<Payload>>(pt.topic);
            recv_count += 1;
            recv_bytes += payload ? payload.value()->size() : 0;
        } else if (type_name == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.load<std::shared_ptr<Payload>>(pt.topic);
            recv_count += 1;
            recv_bytes += payload ? payload.value()->size() : 0;
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}
//////////////////////////////////////////////////////////////////////////////
extern BlackBoardSharedAny& get_BlackBoardSharedAny();

TestReaderProtoSharedPtrAny::TestReaderProtoSharedPtrAny(const TestReaderDescription& desc)
    : TestReader(desc) {
    this->payload_descs = desc.payload_descs;
}

std::pair<size_t, size_t> TestReaderProtoSharedPtrAny::run_receive(void) {
    size_t recv_count = 0, recv_bytes = 0;
    auto& blackboard = get_BlackBoardSharedAny();
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            auto payload = blackboard.getInput<FixedLengthPayload>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->size() : 0;
        } else if (type_name == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.getInput<DynamicLengthPayload>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->size() : 0;
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}
//////////////////////////////////////////////////////////////////////////////
extern BlackBoardIntrusiveVariant<DynamicLengthPayload, FixedLengthPayload>& get_blackBoardIntrusiveVariant();
TestReaderProtoIntrusiveVariant::TestReaderProtoIntrusiveVariant(const TestReaderDescription& desc)
    : TestReader(desc) {
    this->payload_descs = desc.payload_descs;
}

std::pair<size_t, size_t> TestReaderProtoIntrusiveVariant::run_receive(void) {
    size_t recv_count = 0, recv_bytes = 0;
    auto& blackboard = get_blackBoardIntrusiveVariant();
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            auto payload = blackboard.getInput<FixedLengthPayload>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->get()->size() : 0;
        } else if (type_name == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.getInput<DynamicLengthPayload>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->get()->size() : 0;
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}
