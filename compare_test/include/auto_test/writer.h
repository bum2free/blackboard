#ifndef __WRITER_H__
#define __WRITER_H__

#include "auto_test/payload.h"

#include <functional>
#include <map>
#include <vector>
#include <stdint.h>
#include <string>
#include <thread>
#include <time.h>

class WriterDescription {
public:
    std::string name;
    std::vector<PayloadDescription> payload_descs;
    uint32_t interval_ms;
    uint32_t burden_ms;
};

using WriterDescs = std::vector<WriterDescription>;

class Writer {
public:
    Writer(const WriterDescription &desc);

    virtual ~Writer() = default;

    void start(void);
    void run(void);

    virtual std::pair<size_t, size_t> run_send(void) = 0;

protected:
    std::string name;
    uint32_t interval_ms;
    uint32_t burden_ms;
private:
    timer_t timer;
    std::thread timer_thread;
};

class WriterDummy : public Writer {
public:
    WriterDummy(const WriterDescription &desc);
    virtual ~WriterDummy() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::map<std::string, std::function<Payload*(void)>> payload_creators;
};

class WriterProtoSharedPtrAny : public Writer {
public:
    WriterProtoSharedPtrAny(const WriterDescription &desc);

    virtual ~WriterProtoSharedPtrAny() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class WriterProtoIntrusiveVariant : public Writer {
public:
    WriterProtoIntrusiveVariant(const WriterDescription &desc);

    virtual ~WriterProtoIntrusiveVariant() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif
