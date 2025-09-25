#include "pt-shared_any/test_writer_pt_shared_any.h"
#include "pt-shared_any/test_pt_shared_any.h"

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