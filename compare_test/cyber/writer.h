#ifndef __CYBER_WRITER_H__
#define __CYBER_WRITER_H__
#include "cyber/cyber.h"
#include "auto_test/writer.h"
#include "auto_test/payload.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class WriterCyberWrapper : public Writer {
public:
    WriterCyberWrapper(const WriterDescription &desc, bool use_cyber_timestamp = false);

    virtual ~WriterCyberWrapper() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
    std::unique_ptr<apollo::cyber::Node> node_;
    std::map<std::string, std::shared_ptr<apollo::cyber::WriterBase>> writers_;

    std::shared_ptr<apollo::cyber::Timer> cyber_timer_;
};

#endif
