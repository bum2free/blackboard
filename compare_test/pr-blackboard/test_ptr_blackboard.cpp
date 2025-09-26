#include "pr-blackboard/test_pr_blackboard.h"
#include "pr-blackboard/test_reader_pr_blackboard.h"
#include "pr-blackboard/test_writer_pr_blackboard.h"

int test_pr_blackboard(const PayloadDescs& payload_descs,
                          const TestWriterDescs& writer_descs,
                          const TestReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<TestWriter>> writers;
    static std::vector<std::unique_ptr<TestReader>> readers;

    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<TestWriterPrBlackboard>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<TestReaderPrBlackboard>(reader_desc));
        readers.back()->start();
    }

    return 0;
}