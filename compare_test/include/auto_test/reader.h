#ifndef __READER_H__
#define __READER_H__

#include "auto_test/payload.h"

#include <vector>
#include <stdint.h>
#include <string>
#include <thread>
#include <time.h>

class ReaderDescription {
public:
    std::string name;
    std::vector<PayloadDescription> payload_descs;
    uint32_t interval_ms;
    uint32_t burden_ms;
};

using ReaderDescs = std::vector<ReaderDescription>;

class Reader {
public:
    Reader(const ReaderDescription& desc);

    virtual ~Reader() = default;

    void start(void);
    void run(void);

    virtual std::pair<size_t, size_t> run_receive(void) = 0;
protected:
    std::string name;
    uint32_t interval_ms;
    uint32_t burden_ms;
private:
    timer_t timer;
    std::thread timer_thread;
};

class ReaderDummy : public Reader {
public:
    ReaderDummy(const ReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class ReaderProtoSharedPtrAny : public Reader {
public:
    ReaderProtoSharedPtrAny(const ReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class ReaderProtoIntrusiveVariant : public Reader {
public:
    ReaderProtoIntrusiveVariant(const ReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif
