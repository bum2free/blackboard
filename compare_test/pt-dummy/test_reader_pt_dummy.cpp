#include "pt-dummy/test_reader_pt_dummy.h"
#include "dummy_blackboard.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <signal.h>
#include <thread>

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