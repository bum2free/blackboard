#ifndef __TEST_PT_DUMMY_H__
#define __TEST_PT_DUMMY_H__

#include "test_reader.h"
#include "test_writer.h"

int test_dummy_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
#endif