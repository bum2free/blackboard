#include "pr-blackboard/test_writer_pr_blackboard.h"
#include "pr-blackboard/writer.h"

TestWriterPrBlackboard::TestWriterPrBlackboard(const TestWriterDescription &desc)
    : TestWriter(desc), payload_descs(desc.payload_descs) {
}

std::pair<size_t, size_t> TestWriterPrBlackboard::run_send(void) {
    size_t send_count = 0, send_bytes = 0;

    for (const auto& pt : payload_descs) {
        if (pt.type == FixedLengthPayload::NAME()) {
            mega::Writer<FixedLengthPayload> writer(pt.topic);
            std::shared_ptr<FixedLengthPayload> payload = std::make_shared<FixedLengthPayload>(pt.max_size);
            payload->generate();
            writer.Write(payload);
            //blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            std::shared_ptr<DynamicLengthPayload> payload = std::make_shared<DynamicLengthPayload>(pt.max_size);
            
            payload->generate();
            //blackboard.setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return {send_count, send_bytes};
}