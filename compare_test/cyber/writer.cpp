#include "cyber/writer.h"

WriterCyberWrapper::WriterCyberWrapper(const WriterDescription &desc, bool use_cyber_timestamp) :
    Writer(desc) {
    node_ = apollo::cyber::CreateNode(desc.name);
    for (const auto& pt : desc.payload_descs) {
        if (pt.type == FixedLengthPayload::NAME()) {
            auto writer = node_->CreateWriter<FixedLengthPayload>(pt.topic);
            writers_.emplace(pt.topic, writer);
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto writer = node_->CreateWriter<DynamicLengthPayload>(pt.topic);
            writers_.emplace(pt.topic, writer);
        } else {
            assert(false && "Unknown payload type");
        }
    }

    payload_descs = desc.payload_descs;

    if (use_cyber_timestamp) {
        cyber_timer_ = std::make_shared<apollo::cyber::Timer>(
            desc.interval_ms,
            [this]() {
                this->run();
            },
            false);// !oneshot mode
        cyber_timer_->Start();
    }
}

std::pair<size_t, size_t> WriterCyberWrapper::run_send(void) {
    size_t send_count = 0, send_bytes = 0;

    for (const auto& pt : payload_descs) {
        auto it = writers_.find(pt.topic);
        if (it == writers_.end()) {
            continue;
        }

        if (pt.type == FixedLengthPayload::NAME()) {
            auto payload = std::make_shared<FixedLengthPayload>(pt.max_size);
            payload->generate();

            auto writer = std::dynamic_pointer_cast<apollo::cyber::Writer<FixedLengthPayload>>(it->second);
            writer->Write(payload);

            send_count++;
            send_bytes += payload->size();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto payload = std::make_shared<DynamicLengthPayload>(pt.max_size);
            payload->generate();

            auto writer = std::dynamic_pointer_cast<apollo::cyber::Writer<DynamicLengthPayload>>(it->second);
            writer->Write(payload);

            send_count++;
            send_bytes += payload->size();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(send_count, send_bytes);
}
