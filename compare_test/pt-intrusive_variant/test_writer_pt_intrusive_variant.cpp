#include "pt-intrusive_variant/test_writer_pt_intrusive_variant.h"
#include "pt-intrusive_variant/test_pt_intrusive_variant.h"

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