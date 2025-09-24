#ifndef __TEST_WRITER_H__
#define __TEST_WRITER_H__

#include "payload.h"

#include <functional>
#include <map>
#include <vector>
#include <stdint.h>
#include <string>
#include <thread>
#include <time.h>

class TestWriterDescription {
public:
    std::string name;
    std::vector<PayloadDescription> payload_descs;
    uint32_t interval_ms;
    uint32_t burden_ms;
};

using TestWriterDescs = std::vector<TestWriterDescription>;

class TestWriter {
public:
    TestWriter(const TestWriterDescription &desc);

    virtual ~TestWriter() = default;

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

class TestWriterDummy : public TestWriter {
public:
    TestWriterDummy(const TestWriterDescription &desc);
    virtual ~TestWriterDummy() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::map<std::string, std::function<Payload*(void)>> payload_creators;
};

class TestWriterProtoSharedPtrAny : public TestWriter {
public:
    TestWriterProtoSharedPtrAny(const TestWriterDescription &desc);

    virtual ~TestWriterProtoSharedPtrAny() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class TestWriterProtoIntrusiveVariant : public TestWriter {
public:
    TestWriterProtoIntrusiveVariant(const TestWriterDescription &desc);

    virtual ~TestWriterProtoIntrusiveVariant() = default;

    virtual std::pair<size_t, size_t> run_send(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif
