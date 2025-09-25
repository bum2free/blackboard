#ifndef __TEST_READER_PT_DUMMY_H__
#define __TEST_READER_PT_DUMMY_H__

#include "test_reader.h"

class TestReaderDummy : public TestReader {
public:
    TestReaderDummy(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

#endif