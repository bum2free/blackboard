#ifndef __TEST_WRITER_PT_SHARED_ANY_H__
#define __TEST_WRITER_PT_SHARED_ANY_H__
#include "test_writer.h"

class TestWriterProtoSharedPtrAny : public TestWriter {
public:
    TestWriterProtoSharedPtrAny(const TestWriterDescription &desc);

    virtual ~TestWriterProtoSharedPtrAny() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif