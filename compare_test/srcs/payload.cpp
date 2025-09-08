#include "auto_test/payload.h"

#include <cassert>
#include <iostream>
#include <string>
#include <cstring>

void generate_ramdom_buffer(uint8_t* data, size_t size)
{
    //for (size_t i = 0; i < size; ++i) {
    //    data[i] = static_cast<uint8_t>(rand() % 256);
    //}
    memset(data, 0, size); // For simplicity, fill with zeros
}

std::function<Payload*(void)> get_payload_creator_by_name(const std::string& name, size_t size) {
    if (name == FixedLengthPayload::NAME()) {
        return [size]() {
            return FixedLengthPayload::create(size);
        };
    } else if (name == DynamicLengthPayload::NAME()) {
        return [size]() {
            return DynamicLengthPayload::create(size);
        };
    } else {
        std::cerr << "Unknown payload type: " << name << std::endl;
        assert(false && "Unknown payload type");
        return []() {
            return nullptr; // Unknown payload type
        };
    }
}
