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



#endif
