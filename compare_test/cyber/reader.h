#ifndef __CYBER_READER_H__
#define __CYBER_READER_H__
#include "cyber/cyber.h"
#include "auto_test/reader.h"
#include "auto_test/payload.h"

#include <map>
#include <memory>
#include <string>

class ReaderCyberWrapper : public Reader {
public:
    ReaderCyberWrapper(const ReaderDescription& desc, bool async = false);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
    std::unique_ptr<apollo::cyber::Node> node_;
    std::map<std::string, std::shared_ptr<apollo::cyber::ReaderBase>> readers_;
};

#endif
