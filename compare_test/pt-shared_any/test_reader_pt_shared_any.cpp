#include "pt-shared_any/test_reader_pt_shared_any.h"
#include "pt-shared_any/test_pt_shared_any.h"

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
