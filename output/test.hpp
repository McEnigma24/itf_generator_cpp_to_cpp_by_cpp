#pragma once
#include <array>

class msg1
{
    char* payload = nullptr;
    const std::size_t known_payload_size = 1;

public:
    msg1(char* _payload, std::size_t _payload_size)
    {
        if (_payload_size != known_payload_size) { throw std::runtime_error("Incorrect payload size"); }

        if (payload == nullptr) { throw std::runtime_error("Payload is nullptr"); }

        payload = _payload;
        known_payload_size = _payload_size;
    }
    var_a 110;
};
