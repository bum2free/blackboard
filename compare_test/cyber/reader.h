#ifndef __CYBER_READER_H__
#define __CYBER_READER_H__
#include "cyber/cyber.h"
#include "auto_test/reader.h"
#include "auto_test/payload.h"

#include <map>
#include <memory>
#include <string>

class ReaderCyberWrapper : public TestReader {
public:
    ReaderCyberWrapper(const TestReaderDescription& desc, bool async = false);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
    std::unique_ptr<apollo::cyber::Node> node_;
    std::map<std::string, std::shared_ptr<apollo::cyber::ReaderBase>> readers_;
};

class ReaderSharedPtrAnyCyberRecord : public TestReader {
public:
    ReaderSharedPtrAnyCyberRecord(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif
