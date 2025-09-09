#include "auto_test/payload.h"
#include "auto_test/writer.h"
#include "auto_test/reader.h"

#include "cyber/cyber.h"

#include "cyber/writer.h"
#include "cyber/reader.h"

int test_cyber_blackboard(const PayloadDescs& payload_descs,
                          const WriterDescs& writer_descs,
                          const ReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<Writer>> writers;
    static std::vector<std::unique_ptr<Reader>> readers;

    apollo::cyber::Init("cyber_blackboard_test");
    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<WriterCyberWrapper>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<ReaderCyberWrapper>(reader_desc));
        readers.back()->start();
    }

    return 0;
}

int test_cyber_async_blackboard(const PayloadDescs& payload_descs,
                          const WriterDescs& writer_descs,
                          const ReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<Writer>> writers;
    static std::vector<std::unique_ptr<Reader>> readers;

    apollo::cyber::Init("cyber_blackboard_test");
    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<WriterCyberWrapper>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<ReaderCyberWrapper>(reader_desc, true));
    }

    return 0;
}

int test_cyber_norecv_blackboard(const PayloadDescs& payload_descs,
                          const WriterDescs& writer_descs,
                          const ReaderDescs& reader_descs) {
    static std::vector<std::unique_ptr<Writer>> writers;
    static std::vector<std::unique_ptr<Reader>> readers;

    apollo::cyber::Init("cyber_blackboard_test");
    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<WriterCyberWrapper>(writer_desc));
        writers.back()->start();
    }
    return 0;
}

#include "cyber/shared_any_cyber_recoder_blackboard.h"
//////////////////////////////////////////////////////////////////////////////
BlackBoardSharedAnyCyberRecorder& get_BlackBoardSharedAnyCyberRecorder(bool record_mode) {
    static BlackBoardSharedAnyCyberRecorder blackboard(record_mode ? BlackBoardSharedAnyCyberRecorder::Mode::RECORD : BlackBoardSharedAnyCyberRecorder::Mode::PLAYBACK);
    return blackboard;
}

int test_sharedAny_CyberRecord_blackboard(const PayloadDescs& payload_descs,
    const WriterDescs& writer_descs,
    const ReaderDescs& reader_descs,
    bool record_mode)
{
    static std::vector<std::unique_ptr<Writer>> writers;
    static std::vector<std::unique_ptr<Reader>> readers;

    //initial set mode
    get_BlackBoardSharedAnyCyberRecorder(record_mode);

    // Create and run writers
    for (const auto& writer_desc : writer_descs) {
        //printf("Creating writer: %s\n", writer_desc.name.c_str());
        writers.emplace_back(std::make_unique<WriterSharedPtrAnyCyberRecord>(writer_desc));
        writers.back()->start();
    }

    // Create and run readers
    for (const auto& reader_desc : reader_descs) {
        //printf("Creating reader: %s\n", reader_desc.name.c_str());
        readers.emplace_back(std::make_unique<ReaderSharedPtrAnyCyberRecord>(reader_desc));
        readers.back()->start();
    }
    return 0;
}
