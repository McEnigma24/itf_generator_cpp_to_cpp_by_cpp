#include <array>
#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <type_traits>
#include <vector>

using namespace std;

#include "json.hpp" // external lib
#include "math.h"   // internal lib
#include "util.h"   // local

// struct uintRange
// {
//     u64 start;      // można zrobić z zakręcanym looped zakresem, jak przejdziemy 1 więcej niż max to zaczyna od początku
//     u64 stop;       // np. start 64   stop 10   max_val 80
//     u64 max_val;    // wtedy zakres to <64, 80> <0, 10>
// };

// template<typename T>
// struct contiguousNumberRange    // można tego potem użyć jako podstawę i np. dodawać bazę do tego np. +100 i cały range się przesunie
// {
//     T start;    // 0
//     T stop;     // 10
// };

// Pomysł to Inf, tak aby podczas zapisywania oraz odczytywanie używać wygenerowanych funkcji klasowych do obiektów, które w rzeczywistości //
// są upakowane bitowo najciaśniej jak się da
// zajmują najmniej pamięci jak się da -> ogarniamy ile unikatowych liczb trzeba reprezentować
// jedyne co zapisujemy -> to  bo z interfacu już wywnioskujemy jaki to range i dodamy odpowiednią bazę

// int, range = contiguous, no wrap around, start <= stop //

// MSG -> unikatowe wartości zawsze w rangu od <0, x>
// ITF -> baza, którą dodajemy do liczby w MSG'u -> odpowiedni typ, tak aby w nim zapisać po prostu tą liczbę

// Itf generating prep phase //   with this data we get to know how to generate User functions

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

template <typename T>
void packin(const T& variable, char*& output_byte_current, u8& output_bit_current_in_this_byte_left_to_right)
{
    char* input_byte = get_first_byte(variable);
    const u8 input_bit_width = std::bit_width(variable);
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
}

template <u8 bit_width>
using minimal_uint = std::conditional_t<(bit_width <= 8), uint8_t,
                                        std::conditional_t<(bit_width <= 16), uint16_t, std::conditional_t<(bit_width <= 32), uint32_t, uint64_t>>>;

template <u8 bit_width>
minimal_uint<bit_width> unpackin(char* packed_starting_byte, u8 packed_starting_bit_offset) // offset is directly at first bit
{
    minimal_uint<bit_width> result = 0;
    u8 result_current_bit_offset = bitsof(result) - bit_width; // initial result bit offset - to skip empty bits at front
    char* result_current_byte = get_first_byte(result);

    char* packed_current_byte = packed_starting_byte;

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

class uintPack
{
public:
    std::string name;
    u64 start;
    u64 stop;
    u64 numOfNeededBits; // later we sort by this
    u64 base;

    uintPack(std::string name, u64 start,
             u64 stop) // how many numbers we need     <110, 140>   this range needs only 30 numbers -> how many bits do we really need
    {
        this->name = name;
        this->start = start;
        this->stop = stop;

        base = start;

        u64 range = stop - start; // 140 - 110 == 30
        numOfNeededBits = std::bit_width(range);

        var(numOfNeededBits);
    }
};

class SingleMsgGenerator
{
    std::string name;
    std::vector<uintPack> uintPacks;
    u64 numOfNeededBytes;

public:
    SingleMsgGenerator(std::string json_input_file_name)
    {
        // parsing input //

        // stub //
        name = "msg1"; // value range //
        uintPacks.emplace_back("var_a", 110, 140);

        auto totalNumOfNeededBits =
            std::accumulate(uintPacks.begin(), uintPacks.end(), 0, [](u64 sum, const uintPack& pack) { return sum + pack.numOfNeededBits; });
        numOfNeededBytes = (totalNumOfNeededBits + 7) / 8;

        // reordering Packs //
    }

    void generate()
    {
        std::ofstream file("output/test.hpp");

        file << "#pragma once\n\n";
        file << "#include <array> \n";
        file << "\n";
        file << "class " << name << " \n";
        file << "{"
             << "\n";

        file << "\t"
             << "std::array<char, " << numOfNeededBytes << "> payload;"
             << "\n";
        file << "\n";

        for (auto& pack : uintPacks) {}

        file << "};"
             << "\n";

        file.close();
    }
};

// Itf generating phase //   functions that generate cpp code

// User functions //

class pack
{
};

class unpack
{
};

int main()
{
    SingleMsgGenerator g("dupa");
    g.generate();
}

// int main()
// {
//     // Parse JSON from file
//     std::ifstream file("input/test.json");
//     if (!file.is_open())
//     {
//         std::cerr << "Error: Could not open file input/test.json" << std::endl;
//         return 1;
//     }

//     nlohmann::json json_data;
//     file >> json_data;
//     file.close();

//     // Display all values
//     std::cout << "\n=== JSON Content ===" << std::endl;
//     std::cout << json_data.dump(4) << std::endl;

//     std::cout << "\n=== Parsed Values ===" << std::endl;

//     if (json_data.contains("Messages") && json_data["Messages"].is_array())
//     {
//         for (size_t i = 0; i < json_data["Messages"].size(); ++i)
//         {
//             const auto& message = json_data["Messages"][i];

//             std::cout << "\n--- Message " << i << " ---" << std::endl;

//             if (message.contains("tytle")) { std::cout << "Title: " << message["tytle"].get<std::string>() << std::endl; }

//             // Display var_a
//             if (message.contains("var_a"))
//             {
//                 const auto& var_a = message["var_a"];
//                 std::cout << "\nvar_a:" << std::endl;
//                 if (var_a.contains("type")) std::cout << "  type: " << var_a["type"].get<std::string>() << std::endl;
//                 if (var_a.contains("required")) std::cout << "  required: " << var_a["required"].get<bool>() << std::endl;
//                 if (var_a.contains("range"))
//                 {
//                     std::cout << "  range: [" << var_a["range"][0].get<int>() << ", " << var_a["range"][1].get<int>() << "]" << std::endl;
//                 }
//                 if (var_a.contains("default")) std::cout << "  default: " << var_a["default"].get<int>() << std::endl;
//             }

//             // Display var_b
//             if (message.contains("var_b"))
//             {
//                 const auto& var_b = message["var_b"];
//                 std::cout << "\nvar_b:" << std::endl;
//                 if (var_b.contains("type")) std::cout << "  type: " << var_b["type"].get<std::string>() << std::endl;
//                 if (var_b.contains("required")) std::cout << "  required: " << var_b["required"].get<bool>() << std::endl;
//                 if (var_b.contains("range"))
//                 {
//                     std::cout << "  range: [" << var_b["range"][0].get<int>() << ", " << var_b["range"][1].get<int>() << "]" << std::endl;
//                 }
//                 if (var_b.contains("default")) std::cout << "  default: " << var_b["default"].get<int>() << std::endl;
//             }
//         }
//     }

//     std::cout << "\n=== Accessing specific value ===" << std::endl;
//     var(json_data.at("Messages").at(0).at("var_a").at("default").get<int>());

//     return 0;
// }
