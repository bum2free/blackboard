#ifndef __TEST_READER_PT_INTRUSIVE_VARIANT_H__
#define __TEST_READER_PT_INTRUSIVE_VARIANT_H__
#include "test_reader.h"

class TestReaderProtoIntrusiveVariant : public TestReader {
public:
    TestReaderProtoIntrusiveVariant(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif