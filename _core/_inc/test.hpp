#pragma once

#include "static_itf_files.h"

class msg1
{
    char* payload = nullptr;

public:
    static constexpr std::size_t known_payload_size = 7;

    msg1(char* _payload, std::size_t _payload_size)
    {
        if (_payload_size != known_payload_size) { throw std::runtime_error("Incorrect payload size"); }

        if (_payload == nullptr) { throw std::runtime_error("Payload is nullptr"); }

        payload = _payload;

        save_var_a(0);
        save_var_b(100000);
        save_var_c(0);
        save_var_d(0);
        save_var_e(0);
        save_var_f(0);
        save_var_g(0);
    }

    msg1(std::span<char> _payload) : msg1(_payload.data(), _payload.size()) {}

    void save_var_a(const uint32_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 100000000);
        packin<uint32_t, 27>(static_cast<uint32_t>(value - 0), (payload + 0), 0);
    }

    uint32_t get_var_a() const { return unpackin<27>((payload + 0), 0) + 0; }

    void save_var_b(const uint32_t value)
    {
        CRASH_ON_FALSE(100000 <= value && value <= 100010);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 100000), (payload + 3), 3);
    }

    uint32_t get_var_b() const { return unpackin<4>((payload + 3), 3) + 100000; }

    void save_var_c(const uint8_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 10);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 0), (payload + 3), 7);
    }

    uint8_t get_var_c() const { return unpackin<4>((payload + 3), 7) + 0; }

    void save_var_d(const uint8_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 10);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 0), (payload + 4), 3);
    }

    uint8_t get_var_d() const { return unpackin<4>((payload + 4), 3) + 0; }

    void save_var_e(const uint8_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 10);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 0), (payload + 4), 7);
    }

    uint8_t get_var_e() const { return unpackin<4>((payload + 4), 7) + 0; }

    void save_var_f(const uint8_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 10);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 0), (payload + 5), 3);
    }

    uint8_t get_var_f() const { return unpackin<4>((payload + 5), 3) + 0; }

    void save_var_g(const uint8_t value)
    {
        CRASH_ON_FALSE(0 <= value && value <= 10);
        packin<uint8_t, 4>(static_cast<uint8_t>(value - 0), (payload + 5), 7);
    }

    uint8_t get_var_g() const { return unpackin<4>((payload + 5), 7) + 0; }
};
