#include "pr-blackboard/test_reader_pr_blackboard.h"
#include "pr-blackboard/reader.h"


TestReaderPrBlackboard::TestReaderPrBlackboard(const TestReaderDescription& desc)
    : TestReader(desc) {
    this->payload_descs = desc.payload_descs;
}

std::pair<size_t, size_t> TestReaderPrBlackboard::run_receive(void) {
    size_t recv_count = 0, recv_bytes = 0;
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            mega::Reader<FixedLengthPayload> reader(pt.topic);
            std::shared_ptr<FixedLengthPayload> payload;

            if (reader.Read(payload)) {
                // Successfully read the payload
                recv_count += 1;
                recv_bytes += payload ? payload->size() : 0;
            } else {
                // Handle the case where reading failed (e.g., log an error)
            }
        } else if (type_name == DynamicLengthPayload::NAME()) {
            mega::Reader<DynamicLengthPayload> reader(pt.topic);
            std::shared_ptr<DynamicLengthPayload> payload;

            if (reader.Read(payload)) {
                // Successfully read the payload
                recv_count += 1;
                recv_bytes += payload ? payload->size() : 0;
            } else {
                // Handle the case where reading failed (e.g., log an error)
            }
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}
