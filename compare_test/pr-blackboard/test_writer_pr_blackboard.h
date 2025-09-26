#ifndef __TEST_WRITER_PR_BLACKBOARD_H__
#define __TEST_WRITER_PR_BLACKBOARD_H__
#include "test_writer.h"

class TestWriterPrBlackboard : public TestWriter {
public:
    TestWriterPrBlackboard(const TestWriterDescription &desc);

    virtual ~TestWriterPrBlackboard() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif