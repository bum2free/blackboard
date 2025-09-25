#ifndef __TEST_WRITER_PT_DUMMY_H__
#define __TEST_WRITER_PT_DUMMY_H__
#include "test_writer.h"

class TestWriterDummy : public TestWriter {
public:
    TestWriterDummy(const TestWriterDescription &desc);
    virtual ~TestWriterDummy() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::map<std::string, std::function<Payload*(void)>> payload_creators;
};
#endif