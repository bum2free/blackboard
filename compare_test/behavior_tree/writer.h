#ifndef __BEHAVIOR_TREE_WRITER_H__
#define __BEHAVIOR_TREE_WRITER_H__
#include "behaviortree_cpp_v3/bt_factory.h"
#include "auto_test/writer.h"
#include "auto_test/payload.h"

#include <string>
#include <vector>

class WriterBt : public BT::SyncActionNode {
public:
    WriterBt(const std::string& name, const BT::NodeConfiguration& config);

    void set_payload_descs(const std::vector<PayloadDescription>& desc) {
        payload_descs = desc;
    }
    
    std::pair<size_t, size_t> run_send(void);

    virtual BT::NodeStatus tick() override;

    static BT::PortsList providedPorts();
private:
    std::vector<PayloadDescription> payload_descs;
};

class WriterBtWrapper : public TestWriter {
public:
    WriterBtWrapper(const TestWriterDescription &desc, WriterBt *bt_node);

    virtual ~WriterBtWrapper() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    WriterBt *bt_node;
};

#endif
