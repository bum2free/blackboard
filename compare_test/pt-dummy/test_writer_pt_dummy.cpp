#include "pt-dummy/test_writer_pt_dummy.h"
#include "dummy_blackboard.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <signal.h>
#include <thread>

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