#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

void generate_ramdom_buffer(uint8_t* data, size_t size);

class PayloadDescription {
public:
    std::string topic;
    std::string type;
    size_t max_size;
};

using PayloadDescs = std::map<std::string, PayloadDescription>;

class Payload {
public:
    virtual ~Payload() = default;

    virtual void generate(void) = 0;
    virtual size_t size() const = 0;
};

class FixedLengthPayload : public Payload {
public:
    static const char* NAME() {
        return "FixedStruct";
    }
    static FixedLengthPayload* create(size_t max_size) {
        return new FixedLengthPayload(max_size);
    }

    FixedLengthPayload() = default; //Cyber need this

    FixedLengthPayload(size_t max_size) {
        this->max_size = max_size;
        data = new uint8_t[max_size]; //TBD: use allocator
    }
    virtual ~FixedLengthPayload() {
        delete[] data;
    }
    virtual void generate(void) override {
        generate_ramdom_buffer(data, max_size);
    }
    virtual size_t size() const override { return max_size; }
    
private:
    uint8_t *data = nullptr;
    size_t max_size;
};

class DynamicLengthPayload : public Payload {
public:
    static const char* NAME() {
        return "DynamicStruct";
    }

    static DynamicLengthPayload* create(size_t max_size) {
        return new DynamicLengthPayload(max_size);
    }

    DynamicLengthPayload() = default; //Cyber need this

    DynamicLengthPayload(size_t max_size) {
        this->max_size = max_size;
    }

    virtual ~DynamicLengthPayload() = default;
    virtual void generate(void) override {
        size_t length = rand() % (max_size + 1); // Random length between 0 and MAX_SIZE
        data.resize(length);
        generate_ramdom_buffer(data.data(), length);
    }
    virtual size_t size() const override { return data.size(); }
private:
    std::vector<uint8_t> data; //TBD: use allocator
    size_t max_size;
};

std::function<Payload*(void)> get_payload_creator_by_name(const std::string& type_name, size_t size);

#endif
