#ifndef __TEST_WRITER_PT_INTRUSIVE_VARIANT_H__
#define __TEST_WRITER_PT_INTRUSIVE_VARIANT_H__
#include "test_writer.h"

class TestWriterProtoIntrusiveVariant : public TestWriter {
public:
    TestWriterProtoIntrusiveVariant(const TestWriterDescription &desc);

    virtual ~TestWriterProtoIntrusiveVariant() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif