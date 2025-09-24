#include "auto_test/writer.h"
#include "auto_test/cpu_load.h"
#include "auto_test/statistics.h"

#include "dummy_blackboard.h"
#include "shared_any_blackboard.h"
#include "intrusive_variant_blackboard.h"

#include <cstring>
#include <memory>
#include <signal.h>
#include <thread>

static void __cb_posix_timer(union sigval sv) {
    auto* writer = static_cast<TestWriter*>(sv.sival_ptr);
    if (writer) {
        writer->run();
        // Restart the timer for the next interval
        writer->start();
    }
}

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
//////////////////////////////////////////////////////////////////////////////
TestWriterDummy::TestWriterDummy(const TestWriterDescription &desc)
    : TestWriter(desc) {
    for (const auto& pt : desc.payload_descs) {
        auto creator = get_payload_creator_by_name(pt.type, pt.max_size);
        if (creator) {
            payload_creators.emplace(pt.topic, creator);
        }
    }
}

std::pair<size_t, size_t> TestWriterDummy::run_send(void) {
    size_t send_count = 0, send_bytes = 0;
    auto& blackboard = BlackboardDummy::get_instance();
    for (const auto& creator : payload_creators) {
        auto payload = std::shared_ptr<Payload>(creator.second());
        payload->generate();
        blackboard.save(creator.first, payload);

        send_count++;
        send_bytes += payload->size();
    }

    return {send_count, send_bytes};
}
//////////////////////////////////////////////////////////////////////////////
extern BlackBoardSharedAny& get_BlackBoardSharedAny();

TestWriterProtoSharedPtrAny::TestWriterProtoSharedPtrAny(const TestWriterDescription &desc)
    : TestWriter(desc), payload_descs(desc.payload_descs) {
}

std::pair<size_t, size_t> TestWriterProtoSharedPtrAny::run_send(void) {
    size_t send_count = 0, send_bytes = 0;
    auto& blackboard = get_BlackBoardSharedAny();

    for (const auto& pt : payload_descs) {
        if (pt.type == FixedLengthPayload::NAME()) {
            auto payload = blackboard.getOutput<FixedLengthPayload>(pt.topic.c_str(), pt.max_size);
            payload->generate();
            blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.getOutput<DynamicLengthPayload>(pt.topic.c_str(), pt.max_size);
            payload->generate();
            blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return {send_count, send_bytes};
}
//////////////////////////////////////////////////////////////////////////////
BlackBoardIntrusiveVariant<DynamicLengthPayload, FixedLengthPayload>& get_blackBoardIntrusiveVariant();

TestWriterProtoIntrusiveVariant::TestWriterProtoIntrusiveVariant(const TestWriterDescription &desc)
    : TestWriter(desc), payload_descs(desc.payload_descs) {
}

std::pair<size_t, size_t> TestWriterProtoIntrusiveVariant::run_send(void) {
    size_t send_count = 0, send_bytes = 0;
    auto& blackboard = get_blackBoardIntrusiveVariant();

    for (const auto& pt : payload_descs) {
        if (pt.type == FixedLengthPayload::NAME()) {
            auto payload = blackboard.getOutput<FixedLengthPayload>(pt.topic.c_str(), pt.max_size);
            payload->get()->generate();
            blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->get()->size();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.getOutput<DynamicLengthPayload>(pt.topic.c_str(), pt.max_size);
            payload->get()->generate();
            blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->get()->size();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return {send_count, send_bytes};
}
