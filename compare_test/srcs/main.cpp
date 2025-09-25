#include "payload.h"
#include "test_writer.h"
#include "test_reader.h"
#include "statistics.h"

#include "pt-dummy/test_pt_dummy.h"
#include "pt-shared_any/test_pt_shared_any.h"
#include "pt-intrusive_variant/test_pt_intrusive_variant.h"

#include <json.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>

int test_bt_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
int test_cyber_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
int test_cyber_async_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
int test_cyber_norecv_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
int test_sharedAny_CyberRecord_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs,
    bool record_mode);

int main(int argc, char** argv) {
    //parse command line arguments
    //--config <config_file>
    //--bb_type <blackboard_type>
    std::string config_file;
    std::string bb_type;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--config" && i + 1 < argc) {
            config_file = argv[++i];
        } else if (std::string(argv[i]) == "--bb_type" && i + 1 < argc) {
            bb_type = argv[++i];
        }
    }
    if (config_file.empty() || bb_type.empty()) {
        std::cerr << "Usage: " << argv[0] << " --config <config_file> --bb_type <blackboard_type>" << std::endl;
        return 1;
    }

    PayloadDescs payload_descs;
    TestWriterDescs writer_descs;
    TestReaderDescs reader_descs;

    //load configuration from file and using nlohmann::json to parse it
    //for exmaple format:
    /*
{
  "payloads": {
    "topic_1a": {
      "type": "FixStruct",
      "max_size": 32
    },
    "topic_1b": {
      "type": "FixStruct",
      "max_size": 2048
    },
    "topic_2": {
      "type": "DynamicStruct",
      "max_size": 2048
    }
  },
  "writers": [
    {
      "name": "writer1",
      "payloads": ["topic_1a", "topic_1b"],
      "interval_ms": 20,
      "burden_ms": 2
    },
    {
      "name": "writer2",
      "payloads": ["topic_2"],
      "interval_ms": 100,
      "burden_ms": 5
    }
  ],
  "readers": [
    {
      "name": "reader1",
      "payloads": ["topic_2"],
      "interval_ms": 150,
      "burden_ms": 1
    },
    {
      "name": "reader2",
      "payloads": ["topic_2"],
      "interval_ms": 250,
      "burden_ms": 3
    }
  ]
}
*/
    std::ifstream config_stream(config_file);
    if (!config_stream.is_open()) {
        std::cerr << "Failed to open config file: " << config_file << std::endl;
        return 1;
    }
    nlohmann::json config_json;
    try {
        config_stream >> config_json;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Failed to parse config file: " << e.what() << std::endl;
        return 1;
    }
    config_stream.close();
    // Parse payloads
    if (config_json.contains("payloads")) {
        for (const auto& item : config_json["payloads"].items()) {
            PayloadDescription desc;
            desc.topic = item.key();
            desc.type = item.value().value("type", "");
            desc.max_size = item.value().value("max_size", 0);
            payload_descs[desc.topic] = desc;
        }
    } else {
        std::cerr << "No payloads found in config file." << std::endl;
        return 1;
    }
    std::cout << "Loaded " << payload_descs.size() << " payload descriptions." << std::endl;
    // Parse writers
    if (config_json.contains("writers")) {
        for (const auto& item : config_json["writers"]) {
            TestWriterDescription desc;
            desc.name = item.value("name", "");
            for (const auto& payload_name : item.value("payloads", nlohmann::json::array())) {
                if (payload_descs.find(payload_name) != payload_descs.end()) {
                    desc.payload_descs.push_back(payload_descs[payload_name]);
                } else {
                    std::cerr << "Payload " << payload_name << " not found in payload descriptions." << std::endl;
                }
            }
            desc.interval_ms = item.value("interval_ms", 0);
            desc.burden_ms = item.value("burden_ms", 0);
            writer_descs.push_back(desc);
        }
        //resize to 2 for test
        //writer_descs.resize(2);
    } else {
        std::cerr << "No writers found in config file." << std::endl;
        return 1;
    }
    std::cout << "Loaded " << writer_descs.size() << " writer descriptions." << std::endl;
    // Parse readers
    if (config_json.contains("readers")) {
        for (const auto& item : config_json["readers"]) {
            TestReaderDescription desc;
            desc.name = item.value("name", "");
            for (const auto& payload_name : item.value("payloads", nlohmann::json::array())) {
                if (payload_descs.find(payload_name) != payload_descs.end()) {
                    desc.payload_descs.push_back(payload_descs[payload_name]);
                } else {
                    std::cerr << "Payload " << payload_name << " not found in payload descriptions." << std::endl;
                }
            }
            desc.interval_ms = item.value("interval_ms", 0);
            desc.burden_ms = item.value("burden_ms", 0);
            reader_descs.push_back(desc);
        }
        //resize to 1 for test
        //reader_descs.resize(1);
    } else {
        std::cerr << "No readers found in config file." << std::endl;
        return 1;
    }
    std::cout << "Loaded " << reader_descs.size() << " reader descriptions." << std::endl;


    if (bb_type == "shared_any") {
        // Use shared_ptr_pool::any_no_prealloc_blackboard
        std::cout << "Using shared_ptr_pool::any_no_prealloc_blackboard" << std::endl;
        test_sharedAny_blackboard(payload_descs, writer_descs, reader_descs);
    } else if (bb_type == "dummy") {
        // Use dummy blackboard
        std::cout << "Using dummy blackboard" << std::endl;
        test_dummy_blackboard(payload_descs, writer_descs, reader_descs);
    } else if (bb_type == "intrusive_variant") {
        // Use intrusive_ptr_pool::intrusive_variant_blackboard
        std::cout << "Using intrusive_ptr_pool::intrusive_variant_blackboard" << std::endl;
        test_intrusiveVariant_blackboard(payload_descs, writer_descs, reader_descs);
#ifdef ENABLE_BT_BLACKBOARD
    } else if (bb_type == "bt") {
        // Use bh_blackboard
        std::cout << "Using bt_blackboard" << std::endl;
        test_bt_blackboard(payload_descs, writer_descs, reader_descs);
#endif
#ifdef ENABLE_CYBER_BLACKBOARD
    } else if (bb_type == "cyber") {
        // Use cyber::blackboard
        std::cout << "Using cyber_blackboard" << std::endl;
        test_cyber_blackboard(payload_descs, writer_descs, reader_descs);
    } else if (bb_type == "cyber_async") {
        // Use cyber::blackboard with async reader callback
        std::cout << "Using cyber_async_blackboard" << std::endl;
        test_cyber_async_blackboard(payload_descs, writer_descs, reader_descs);
    } else if (bb_type == "cyber_norecv") {
        // Use cyber::blackboard with no recv reader
        std::cout << "Using cyber_norecv_blackboard" << std::endl;
        test_cyber_norecv_blackboard(payload_descs, writer_descs, reader_descs);
    } else if (bb_type == "cyber_record") {
        // Use cyber::blackboard with no recv reader
        std::cout << "Using shared_any_blackboard with cyber record" << std::endl;
        test_sharedAny_CyberRecord_blackboard(payload_descs, writer_descs, reader_descs, true);
    } else if (bb_type == "cyber_playback") {
        // Use cyber::blackboard with no recv reader
        std::cout << "Using shared_any_blackboard with cyber playback" << std::endl;
        test_sharedAny_CyberRecord_blackboard(payload_descs, writer_descs, reader_descs, false);
#endif
    } else {
        std::cerr << "Unknown blackboard type: " << bb_type << std::endl;
        return 1;
    }

    //main thread idle running
    auto& statistics = Statistics::get_instance();
    const uint64_t interval_s = 3;
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_s * 1000));

        printf("======== Statistics ========\n");
        printf("%d readers, %d writers\n",
                static_cast<int>(reader_descs.size()),
                static_cast<int>(writer_descs.size()));
        printf("Send: %lu bytes/s, %lu packets/s\n",
                statistics.reset_send_bytes() / interval_s,
                statistics.reset_send_count() / interval_s);
        printf("Recv: %lu bytes/s, %lu packets/s\n",
                statistics.reset_recv_bytes() / interval_s,
                statistics.reset_recv_count() / interval_s);
        printf("CPU Time: %lu ms/s\n",
                statistics.reset_cpu_time_us() / 1000 / interval_s);
        printf("System Heap Alloc: %lu bytes/s, %lu allocs/s, %lu frees/s\n",
                statistics.reset_system_heap_alloc_bytes() / interval_s,
                statistics.reset_system_heap_alloc_count() / interval_s,
                statistics.reset_system_heap_free_count() / interval_s);
        printf("============================\n");
    }

    return 0;
}

//////////////////////For system heap monitor///////////////////
////////////////////////////////////////////////////////////////
void* operator new(std::size_t size) {
    auto& statistics = Statistics::get_instance();
    void* ptr = std::malloc(size);
    //std::cout << "[new] size=" << size << " -> " << ptr << std::endl;
    statistics.add_system_heap_alloc_bytes(size);
    statistics.add_system_heap_alloc_count(1);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    auto& statistics = Statistics::get_instance();
    statistics.add_system_heap_free_count(1);
    //std::cout << "[delete] ptr=" << ptr << std::endl;
    std::free(ptr);
}

void* operator new[](std::size_t size) {
    auto& statistics = Statistics::get_instance();
    void* ptr = std::malloc(size);
    //std::cout << "[new[]] size=" << size << " -> " << ptr << std::endl;
    statistics.add_system_heap_alloc_bytes(size);
    statistics.add_system_heap_alloc_count(1);
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    auto& statistics = Statistics::get_instance();
    //std::cout << "[delete[]] ptr=" << ptr << std::endl;
    statistics.add_system_heap_free_count(1);
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t size) noexcept {
    auto& statistics = Statistics::get_instance();
    statistics.add_system_heap_free_count(1);
    //std::cout << "[delete with size] ptr=" << ptr << ", size=" << size << std::endl;
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept {
    auto& statistics = Statistics::get_instance();
    statistics.add_system_heap_free_count(1);
    //std::cout << "[delete[] with size] ptr=" << ptr << ", size=" << size << std::endl;
    std::free(ptr);
}
