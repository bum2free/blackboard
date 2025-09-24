#include "cyber/reader.h"
#include "auto_test/cpu_load.h"
#include "auto_test/statistics.h"

#include "shared_any_cyber_recoder_blackboard.h"
#include <chrono>

ReaderCyberWrapper::ReaderCyberWrapper(const TestReaderDescription& desc, bool async) :
    TestReader(desc) {
    node_ = apollo::cyber::CreateNode(desc.name);
    for (const auto& pt : desc.payload_descs) {
        std::shared_ptr<apollo::cyber::ReaderBase> reader;
        if (pt.type == FixedLengthPayload::NAME()) {
            if (async) {
                auto burden_ms = desc.burden_ms;
                reader = node_->CreateReader<FixedLengthPayload>(pt.topic, [burden_ms](const std::shared_ptr<FixedLengthPayload>& msg) {
                    auto& statistics = Statistics::get_instance();
                    uint64_t duration_us = burden_ms * 1000; // Convert milliseconds to microseconds
                    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
                    size_t recv_bytes =  msg->size();
                    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
                    uint64_t elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                    if (elapsed_time_us < duration_us) {
                        elapsed_time_us += consume_cpu_in_us(duration_us - elapsed_time_us);
                    }
                    statistics.add_recv_count(1);
                    statistics.add_recv_bytes(recv_bytes);
                    statistics.add_cpu_time_us(elapsed_time_us);
                });
            } else {
                //FIXME! although in synchronous mode, we still need to provide a callback function, or we get abnormal high cpu usage
                reader = node_->CreateReader<FixedLengthPayload>(pt.topic, [](const std::shared_ptr<FixedLengthPayload>& msg) {
                    //assert(false && "Synchronous reading should not have a callback");
                    // No-op callback for synchronous reading
                });
            }

            readers_.emplace(pt.topic, reader);
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            if (async) {
                auto burden_ms = desc.burden_ms;
                reader = node_->CreateReader<DynamicLengthPayload>(pt.topic, [burden_ms](const std::shared_ptr<DynamicLengthPayload>& msg) {
                    auto& statistics = Statistics::get_instance();
                    uint64_t duration_us = burden_ms * 1000; // Convert milliseconds to microseconds
                    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
                    size_t recv_bytes =  msg->size();
                    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
                    uint64_t elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                    if (elapsed_time_us < duration_us) {
                        elapsed_time_us += consume_cpu_in_us(duration_us - elapsed_time_us);
                    }
                    statistics.add_recv_count(1);
                    statistics.add_recv_bytes(recv_bytes);
                    statistics.add_cpu_time_us(elapsed_time_us);
                });
            } else {
                //FIXME! although in synchronous mode, we still need to provide a callback function, or we get abnormal high cpu usage
                reader = node_->CreateReader<DynamicLengthPayload>(pt.topic, [](const std::shared_ptr<DynamicLengthPayload>& msg) {
                    //assert(false && "Synchronous reading should not have a callback");
                    // No-op callback for synchronous reading
                });
            }

            readers_.emplace(pt.topic, reader);
        } else {
            assert(false && "Unknown payload type");
        }
    }

    payload_descs = desc.payload_descs;

    node_->Observe();
}

std::pair<size_t, size_t> ReaderCyberWrapper::run_receive(void)
{
    size_t recv_count = 0, recv_bytes = 0;
    for (const auto& pt : payload_descs) {
        auto it = readers_.find(pt.topic);
        if (it == readers_.end()) {
            continue;
        }

        if (pt.type == FixedLengthPayload::NAME()) {
            auto reader = std::dynamic_pointer_cast<apollo::cyber::Reader<FixedLengthPayload>>(it->second);
            auto payload = reader->GetLatestObserved();
            if (payload) {
                recv_count += 1;
                recv_bytes += payload->size();
            } else {
                //printf("No payload received on topic %s\n", pt.topic.c_str());
            }
            reader->Observe();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto reader = std::dynamic_pointer_cast<apollo::cyber::Reader<DynamicLengthPayload>>(it->second);
            auto payload = reader->GetLatestObserved();
            if (payload) {
                recv_count += 1;
                recv_bytes += payload->size();
            } else {
                //printf("No payload received on topic %s\n", pt.topic.c_str());
            }
            reader->Observe();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}

//////////////////////////////////////////////////////////////////////////////
#include "cyber/examples/proto/examples.pb.h"
#include "cyber/time/time.h"

using apollo::cyber::Time;
using apollo::cyber::examples::proto::Chatter;

extern BlackBoardSharedAnyCyberRecorder& get_BlackBoardSharedAnyCyberRecorder(bool record_mode = true);

ReaderSharedPtrAnyCyberRecord::ReaderSharedPtrAnyCyberRecord(const TestReaderDescription& desc)
    : TestReader(desc) {
    this->payload_descs = desc.payload_descs;
}

std::pair<size_t, size_t> ReaderSharedPtrAnyCyberRecord::run_receive(void) {
    size_t recv_count = 0, recv_bytes = 0;
    auto& blackboard = get_BlackBoardSharedAnyCyberRecorder();
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            auto payload = blackboard.getInput<Chatter>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->ByteSizeLong() : 0;
        } else if (type_name == DynamicLengthPayload::NAME()) {
            auto payload = blackboard.getInput<Chatter>(pt.topic.c_str());
            recv_count += 1;
            recv_bytes += payload ? payload->ByteSizeLong() : 0;
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}
