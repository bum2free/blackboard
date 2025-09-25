#ifndef __TEST_PT_INTRUSIVE_VARIANT_H__
#define __TEST_PT_INTRUSIVE_VARIANT_H__

#include "intrusive_variant_blackboard.h"
#include "test_reader.h"
#include "test_writer.h"

BlackBoardIntrusiveVariant<DynamicLengthPayload, FixedLengthPayload>& get_blackBoardIntrusiveVariant();

int test_intrusiveVariant_blackboard(const PayloadDescs& payload_descs,
    const TestWriterDescs& writer_descs,
    const TestReaderDescs& reader_descs);
#endif