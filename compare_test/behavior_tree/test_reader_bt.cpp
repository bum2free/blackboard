#include "behavior_tree/test_reader_bt.h"

ReaderBt::ReaderBt(const std::string& name, const BT::NodeConfiguration& config)
    : BT::SyncActionNode(name, config) {
}

std::pair<size_t, size_t> ReaderBt::run_receive(void)
{
    size_t recv_count = 0, recv_bytes = 0;
    for (const auto& pt : payload_descs) {
        auto& type_name = pt.type;
        if (type_name == FixedLengthPayload::NAME()) {
            std::shared_ptr<FixedLengthPayload> payload;
            auto ret = getInput(pt.topic.c_str(), payload);
            if (ret) {
                recv_count += 1;
                recv_bytes += payload ? payload->size() : 0;
            }
        } else if (type_name == DynamicLengthPayload::NAME()) {
            std::shared_ptr<DynamicLengthPayload> payload;
            auto ret = getInput(pt.topic.c_str(), payload);
            if (ret) {
                recv_count += 1;
                recv_bytes += payload ? payload->size() : 0;
            }
        } else {
            assert(false && "Unknown payload type");
        }
    }

    return std::make_pair(recv_count, recv_bytes);
}

BT::NodeStatus ReaderBt::tick()
{
    return BT::NodeStatus::SUCCESS;
}

extern BT::PortsList get_bt_input_ports();
BT::PortsList ReaderBt::providedPorts()
{
    return get_bt_input_ports();
}

ReaderBtWrapper::ReaderBtWrapper(const TestReaderDescription& desc, ReaderBt *bt_node)
    : TestReader(desc), bt_node(bt_node)
{
}

std::pair<size_t, size_t> ReaderBtWrapper::run_receive(void) {
    return bt_node->run_receive();
}
