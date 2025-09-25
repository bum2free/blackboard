#ifndef __TEST_PT_SHARED_ANY_H__
#define __TEST_PT_SHARED_ANY_H__

#include "shared_any_blackboard.h"
#include "test_reader.h"
#include "test_writer.h"

BlackBoardSharedAny& get_BlackBoardSharedAny();

int test_sharedAny_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
#endif