#ifndef __TEST_READER_PT_SHARED_ANY_H__
#define __TEST_READER_PT_SHARED_ANY_H__
#include "test_reader.h"

class TestReaderProtoSharedPtrAny : public TestReader {
public:
    TestReaderProtoSharedPtrAny(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif