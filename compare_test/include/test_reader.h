#ifndef __TEST_READER_H__
#define __TEST_READER_H__

#include "payload.h"

#include <vector>
#include <stdint.h>
#include <string>
#include <thread>
#include <time.h>

class TestReaderDescription {
public:
    std::string name;
    std::vector<PayloadDescription> payload_descs;
    uint32_t interval_ms;
    uint32_t burden_ms;
};

using TestReaderDescs = std::vector<TestReaderDescription>;

class TestReader {
public:
    TestReader(const TestReaderDescription& desc);

    virtual ~TestReader() = default;

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

class TestReaderDummy : public TestReader {
public:
    TestReaderDummy(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class TestReaderProtoSharedPtrAny : public TestReader {
public:
    TestReaderProtoSharedPtrAny(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};

class TestReaderProtoIntrusiveVariant : public TestReader {
public:
    TestReaderProtoIntrusiveVariant(const TestReaderDescription& desc);

    virtual std::pair<size_t, size_t> run_receive(void) override;
private:
    std::vector<PayloadDescription> payload_descs;
};
#endif
