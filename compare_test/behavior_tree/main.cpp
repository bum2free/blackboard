#include "payload.h"
#include "test_writer.h"
#include "test_reader.h"
#include "behavior_tree/test_writer_bt.h"
#include "behavior_tree/test_reader_bt.h"

#include "behaviortree_cpp_v3/bt_factory.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

#include <map>
#include <memory>

static BT::PortsList s_output_ports;
static BT::PortsList s_input_ports;

BT::PortsList get_bt_output_ports()
{
    return s_output_ports;
}

BT::PortsList get_bt_input_ports()
{
    return s_input_ports;
}

int test_bt_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs)
{
    static BT::BehaviorTreeFactory factory;
    static std::vector<std::unique_ptr<TestWriter>> writers;
    static std::vector<std::unique_ptr<TestReader>> readers;

    //construct BT::Portslist for writer
    {
        std::map<std::string, std::string> writer_payloads_desc;
        for (const auto& writer_desc : writer_descs) {
            for (const auto& payload_desc : writer_desc.payload_descs) {
                writer_payloads_desc[payload_desc.topic] = payload_desc.type;
            }
        }

        for (const auto& desc : writer_payloads_desc) {
            if (desc.second == FixedLengthPayload::NAME()) {
                s_output_ports.insert(
                    BT::OutputPort<std::shared_ptr<FixedLengthPayload>>(desc.first, desc.second));
            } else if (desc.second == DynamicLengthPayload::NAME()) {
                s_output_ports.insert(
                    BT::OutputPort<std::shared_ptr<DynamicLengthPayload>>(desc.first, desc.second));
            } else {
                assert(false && "Unknown payload type in writer description");
            }
        }
    }
    //construct BT::Portslist for reader
    {
        std::map<std::string, std::string> reader_payloads_desc;
        for (const auto& reader_desc : reader_descs) {
            for (const auto& payload_desc : reader_desc.payload_descs) {
                reader_payloads_desc[payload_desc.topic] = payload_desc.type;
            }
        }

        for (const auto& desc : reader_payloads_desc) {
            if (desc.second == FixedLengthPayload::NAME()) {
                s_input_ports.insert(
                    BT::InputPort<std::shared_ptr<FixedLengthPayload>>(desc.first, desc.second));
            } else if (desc.second == DynamicLengthPayload::NAME()) {
                s_input_ports.insert(
                    BT::InputPort<std::shared_ptr<DynamicLengthPayload>>(desc.first, desc.second));
            } else {
                assert(false && "Unknown payload type in reader description");
            }
        }
    }

    // register behavior tree nodes
    for (const auto& writer_desc : writer_descs) {
        factory.registerNodeType<WriterBt>(writer_desc.name);
    }
    for (const auto& reader_desc : reader_descs) {
        factory.registerNodeType<ReaderBt>(reader_desc.name);
    }

    static BT::Tree tree;
    //construct the xml tree
    {
        rapidxml::xml_document<> doc;

        // Create the <root> node with its attribute
        rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "root");
        root->append_attribute(doc.allocate_attribute("main_tree_to_execute", "MainTree"));
        doc.append_node(root);

        // Create the <BehaviorTree> node with its ID
        rapidxml::xml_node<>* behaviorTree = doc.allocate_node(rapidxml::node_element, "BehaviorTree");
        behaviorTree->append_attribute(doc.allocate_attribute("ID", "MainTree"));
        root->append_node(behaviorTree);

        // Create the <KeepRunningUntilFailure> node
        rapidxml::xml_node<>* keepRunning = doc.allocate_node(rapidxml::node_element, "KeepRunningUntilFailure");
        behaviorTree->append_node(keepRunning);

        // Create the <Sequence> node with its name
        rapidxml::xml_node<>* sequence = doc.allocate_node(rapidxml::node_element, "Sequence");
        sequence->append_attribute(doc.allocate_attribute("name", "root"));
        keepRunning->append_node(sequence);

        for (const auto& writer_desc : writer_descs) {
            // Create a writer node with its name
            rapidxml::xml_node<>* writerNode = doc.allocate_node(rapidxml::node_element, writer_desc.name.c_str());
            // Add input/output ports as attributes
            for (const auto& payload_desc : writer_desc.payload_descs) {
                std::string port_attr = "{" + payload_desc.topic + "}";
                char *new_port_attr = doc.allocate_string(port_attr.c_str());
                writerNode->append_attribute(doc.allocate_attribute(payload_desc.topic.c_str(), new_port_attr));
            }
            sequence->append_node(writerNode);
        }
        for (const auto& reader_desc : reader_descs) {
            // Create a reader node with its name
            rapidxml::xml_node<>* readerNode = doc.allocate_node(rapidxml::node_element, reader_desc.name.c_str());
            // Add input/output ports as attributes
            for (const auto& payload_desc : reader_desc.payload_descs) {
                std::string port_attr = "{" + payload_desc.topic + "}";
                char *new_port_attr = doc.allocate_string(port_attr.c_str());
                readerNode->append_attribute(doc.allocate_attribute(payload_desc.topic.c_str(), new_port_attr));
            }
            sequence->append_node(readerNode);
        }

        // Print the XML document to a string
        std::string xml_string;
        rapidxml::print(std::back_inserter(xml_string), doc, 0);
        //rapidxml::print(std::back_inserter(xml_string), doc, rapidxml::print_no_indenting);

        // Output the resulting XML
        //std::cout << xml_string << std::endl;

        tree = factory.createTreeFromText(xml_string);

        doc.clear(); // Free allocated memory
    }

    auto& nodes = tree.nodes;
    std::map<std::string, WriterBt*> writer_nodes;
    std::map<std::string, ReaderBt*> reader_nodes;
    //printf("Number of nodes in the tree: %zu\n", nodes.size());
    for (BT::TreeNode::Ptr& node : nodes)
    {
        //printf("Node: %s, Type: %d\n", node->name().c_str(), (int)node->type());
        //find it in writer_descs
        auto it_writer = std::find_if(writer_descs.begin(), writer_descs.end(),
            [&node](const TestWriterDescription& desc) { return desc.name == node->name(); });
        if (it_writer != writer_descs.end()) {
            auto writer_node = dynamic_cast<WriterBt*>(node.get());
            if (writer_node) {
                writer_node->set_payload_descs(it_writer->payload_descs);
                writer_nodes[it_writer->name] = writer_node;
            } else {
                assert(false && "Failed to cast to WriterBt");
            }
            continue;
        }
        //find it in reader_descs
        auto it_reader = std::find_if(reader_descs.begin(), reader_descs.end(),
            [&node](const TestReaderDescription& desc) { return desc.name == node->name(); });
        if (it_reader != reader_descs.end()) {
            auto reader_node = dynamic_cast<ReaderBt*>(node.get());
            if (reader_node) {
                reader_node->set_payload_descs(it_reader->payload_descs);
                reader_nodes[it_reader->name] = reader_node;
            } else {
                assert(false && "Failed to cast to ReaderBt");
            }
            continue;
        }
    }

    //start writers
    for (const auto& writer_desc : writer_descs) {
        writers.emplace_back(
            std::make_unique<WriterBtWrapper>(writer_desc, writer_nodes[writer_desc.name]));
        writers.back()->run_send();
        writers.back()->start();
    }
    
    //start readers
    for (const auto& reader_desc : reader_descs) {
        readers.emplace_back(
            std::make_unique<ReaderBtWrapper>(reader_desc, reader_nodes[reader_desc.name]));
        readers.back()->start();
    }

    return 0;
}
