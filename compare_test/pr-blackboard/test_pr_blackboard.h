#ifndef __TEST_PR_BLACKBOARD_H__
#define __TEST_PR_BLACKBOARD_H__

#include "test_reader.h"
#include "test_writer.h"

int test_pr_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
#endif