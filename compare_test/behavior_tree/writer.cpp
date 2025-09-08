#include <memory>

#include "behavior_tree/writer.h"

WriterBt::WriterBt(const std::string& name, const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name, config) {
}

std::pair<size_t, size_t> WriterBt::run_send(void) {
    size_t send_count = 0, send_bytes = 0;

    for (const auto& pt : payload_descs) {
        if (pt.type == FixedLengthPayload::NAME()) {
            auto payload = std::make_shared<FixedLengthPayload>(pt.max_size);
            payload->generate();
            setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else if (pt.type == DynamicLengthPayload::NAME()) {
            auto payload = std::make_shared<DynamicLengthPayload>(pt.max_size);
            payload->generate();
            setOutput(pt.topic.c_str(), payload);

            send_count++;
            send_bytes += payload->size();
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return {send_count, send_bytes};
}

BT::NodeStatus WriterBt::tick()
{
    return BT::NodeStatus::SUCCESS;
}
    
extern BT::PortsList get_bt_output_ports();
BT::PortsList WriterBt::providedPorts()
{
    return get_bt_output_ports();
}

WriterBtWrapper::WriterBtWrapper(const WriterDescription &desc, WriterBt *bt_node)
    : Writer(desc), bt_node(bt_node)
{
}
    
std::pair<size_t, size_t> WriterBtWrapper::run_send(void) {
    return bt_node->run_send();
}
