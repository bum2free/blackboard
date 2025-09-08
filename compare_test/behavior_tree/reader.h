#ifndef __BEHAVIOR_TREE_READER_H__
#define __BEHAVIOR_TREE_READER_H__
#include "behaviortree_cpp_v3/bt_factory.h"
#include "auto_test/reader.h"
#include "auto_test/payload.h"

#include <string>
#include <vector>

class ReaderBt : public BT::SyncActionNode {
public:
    ReaderBt(const std::string& name, const BT::NodeConfiguration& config);

    void set_payload_descs(const std::vector<PayloadDescription>& desc) {
        payload_descs = desc;
    }

    std::pair<size_t, size_t> run_receive(void);

    virtual BT::NodeStatus tick() override;

    static BT::PortsList providedPorts();
private:
    std::vector<PayloadDescription> payload_descs;
};

class ReaderBtWrapper : public Reader {
public:
    ReaderBtWrapper(const ReaderDescription& desc, ReaderBt *bt_node);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    ReaderBt *bt_node;
};
#endif
