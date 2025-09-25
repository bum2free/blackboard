#include "pt-intrusive_variant/test_pt_intrusive_variant.h"
#include "pt-intrusive_variant/test_reader_pt_intrusive_variant.h"
#include "pt-intrusive_variant/test_writer_pt_intrusive_variant.h"

BlackBoardIntrusiveVariant<DynamicLengthPayload, FixedLengthPayload>& get_blackBoardIntrusiveVariant() {
    static BlackBoardIntrusiveVariant<DynamicLengthPayload, FixedLengthPayload> blackboard;
    return blackboard;
}

int test_intrusiveVariant_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<TestWriter>> writers;
    static std::vector<std::unique_ptr<TestReader>> readers;

    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<TestWriterProtoIntrusiveVariant>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<TestReaderProtoIntrusiveVariant>(reader_desc));
        readers.back()->start();
    }

    return 0;
}