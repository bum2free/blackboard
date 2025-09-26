#ifndef __TEST_READER_PR_BLACKBOARD_H__
#define __TEST_READER_PR_BLACKBOARD_H__
#include "test_reader.h"

class TestReaderPrBlackboard : public TestReader {
public:
    TestReaderPrBlackboard(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif