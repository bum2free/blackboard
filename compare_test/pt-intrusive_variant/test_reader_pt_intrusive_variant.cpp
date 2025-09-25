#include "pt-intrusive_variant/test_reader_pt_intrusive_variant.h"
#include "pt-intrusive_variant/test_pt_intrusive_variant.h"

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
