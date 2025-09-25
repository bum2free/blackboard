#include "pt-dummy/test_pt_dummy.h"
#include "pt-dummy/test_reader_pt_dummy.h"
#include "pt-dummy/test_writer_pt_dummy.h"

int test_dummy_blackboard(const PayloadDescs& payload_descs,
                          const TestWriterDescs& writer_descs,
                          const TestReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<TestWriter>> writers;
    static std::vector<std::unique_ptr<TestReader>> readers;

    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<TestWriterDummy>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<TestReaderDummy>(reader_desc));
        readers.back()->start();
    }

    return 0;
}