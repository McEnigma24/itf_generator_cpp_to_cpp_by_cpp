#pragma once

#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>

#undef bits
#undef bitss
#undef var
#undef varr
#undef line
#undef CRASH_log
#undef CRASH_ON_NULL
#undef CRASH_ON_FALSE
#undef CRASH_ON_TRUE

#define bits(x)                                                                                                                                      \
    {                                                                                                                                                \
        bitset<sizeof(x) * 8> bit(x);                                                                                                                \
        cout << #x << " = " << bit << '\n';                                                                                                          \
    }
#define bitss(x)                                                                                                                                     \
    {                                                                                                                                                \
        bitset<sizeof(x) * 8> bit(x);                                                                                                                \
        cout << #x << " = " << bit << ' ';                                                                                                           \
    }
#define var(x)  cout << #x << " = " << (int64_t)x << '\n';
#define varr(x) cout << #x << " = " << (int64_t)x << ' ';
#define line(x) cout << x << '\n';

#define CRASH_log(x)                                                                                                                                 \
    std::cerr << "\n\n\n" << __FILENAME__ << ":" << __LINE__ << " - CRASHED - " << x << "\n\n";                                                      \
    std::abort();
#define CRASH_ON_NULL(x)                                                                                                                             \
    if ((x) == nullptr) { CRASH_log(#x << " is nullptr"); }
#define CRASH_ON_FALSE(x)                                                                                                                            \
    if ((x) == false) { CRASH_log(#x << " is false"); }
#define CRASH_ON_TRUE(x)                                                                                                                             \
    if ((x) == true) { CRASH_log(#x << " is true"); }

template <typename T>
i8 bitsof(const T& variable)
{
    return sizeof(T) * 8;
}

template <typename T>
char* get_first_byte(const T& variable)
{
    if constexpr ((std::endian::native) == (std::endian::big)) { return (char*)&variable; }
    else if constexpr ((std::endian::native) == (std::endian::little)) { return (char*)&variable + sizeof(T) - 1; }
}

void move_to_next_byte(char*& byte, u8 amount = 1)
{
    if constexpr ((std::endian::native) == (std::endian::big)) { byte += amount; }
    else if constexpr ((std::endian::native) == (std::endian::little)) { byte -= amount; }
}

// IMPROVEMENT //
//
// - jak mamy cały 1 byte do odpakowania to możemy po prostu cały przenieść, bez wyciągania konkretnych bitów
//    to można by w IF'ach dodać, w momencie przechodzenia do nowego, to sprawdzamy czy nie można by ruszyć całości
//    jak można to ruszamy tyle razy ile się da i wracamy do przepisywania bit po bicie
//
// - można w ogóle nie iść 1 po 2, tylko obliczać ile bitów mamy do ruszenia i np. robić maskę na 6 bitów

template <typename T, u8 input_max_bit_width>
tuple<char*, u8> packin(const T& variable, char* _output_byte_current, const u8 _output_bit_current_in_this_byte_left_to_right)
{
    char* output_byte_current = _output_byte_current;
    u8 output_bit_current_in_this_byte_left_to_right = _output_bit_current_in_this_byte_left_to_right;

    char* input_byte = get_first_byte(variable);
    const auto input_bit_width = input_max_bit_width;
    var(input_bit_width);

    // advance past initial empty bytes //
    {
        auto variable_total_bit_length = bitsof(variable);
        i8 bytes_to_advance = (variable_total_bit_length - input_bit_width) / 8;
        move_to_next_byte(input_byte, bytes_to_advance);
    }

    for (u8 i = (input_bit_width - 1); i >= 0; i--)
    {
        varr(i);

        // extract //
        u8 input_mask = u8(1) << (i % 8);
        bitss(input_mask);
        bitss((*input_byte));
        u8 input_value = (bool)((*input_byte) & input_mask);
        varr(input_value);

        // insert //
        (*output_byte_current) |= (input_value << (7 - output_bit_current_in_this_byte_left_to_right));

        if (++output_bit_current_in_this_byte_left_to_right == 8)
        {
            output_byte_current++;
            output_bit_current_in_this_byte_left_to_right = 0;
        }

        if (i == 0) { break; }
        if ((i % 8) == 0) { move_to_next_byte(input_byte); }

        line("");
    }
    line("");
    line("--------------");

    return {output_byte_current, output_bit_current_in_this_byte_left_to_right};
}

template <u8 bit_width>
using minimal_uint = std::conditional_t<(bit_width <= 8), uint8_t,
                                        std::conditional_t<(bit_width <= 16), uint16_t, std::conditional_t<(bit_width <= 32), uint32_t, uint64_t>>>;

template <u8 bit_width>
minimal_uint<bit_width> unpackin(const char* packed_current_byte, u8 packed_starting_bit_offset) // offset is directly at first bit
{
    minimal_uint<bit_width> result = 0;
    u8 result_current_bit_offset = bitsof(result) - bit_width; // initial result bit offset - to skip empty bits at front
    char* result_current_byte = get_first_byte(result);

    // char* packed_current_byte = const_cast<char*>(packed_starting_byte);

    for (u8 i = 0; i < bit_width; i++)
    {
        // extract //
        u8 packed_mask = u8(1) << (7 - packed_starting_bit_offset);
        u8 packed_value = (bool)((*packed_current_byte) & packed_mask);

        bitss(packed_mask);
        bitss((*packed_current_byte));
        varr(packed_value);

        // insert //
        (*result_current_byte) |= (packed_value << (7 - result_current_bit_offset));

        if (++result_current_bit_offset == 8)
        {
            result_current_bit_offset = 0;
            move_to_next_byte(result_current_byte);
        }
        if (++packed_starting_bit_offset == 8)
        {
            packed_starting_bit_offset = 0;
            packed_current_byte++;
        }

        line("");
    }
    line("");
    line("--------------");

    return result;
}

// int main()
// {
//     srand(time(NULL));

//     uint16_t a = 0;
//     bits(a);

//     char tab[3] = {0};
//     char* output_byte_current = tab;
//     u8 output_bit_current = 0;
//     { int i=1; for(const auto& c : tab) { cout << i++ << " "; bits(c); } }

//     // packin //
//     a = 2047 - 1 - 32; bits(a);
//     packin(a, output_byte_current, output_bit_current);
//     { int i=1; for(const auto& c : tab) { cout << i++ << " "; bits(c); } }

//     a = 255 - 64; bits(a);
//     packin(a, output_byte_current, output_bit_current);
//     { int i=1; for(const auto& c : tab) { cout << i++ << " "; bits(c); } }

//     line("");line("");line("");line("");line("");line("");

//     // unpackin //
//     char* packed_byte_current = tab;
//     auto first = unpackin<11>(packed_byte_current, 0);                       var(first);
//     auto second = unpackin<8>(++packed_byte_current, 3);                     var(second);

//     line("");line("");line("");line("");line("");line("");
//     return 0;
// }