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

class scopedTab
{
    std::string og;
    std::string& s;

public:
    scopedTab(std::string& s) : og(s), s(s) { s += "\t"; }

    ~scopedTab() { s = og; }
};

class uintPack
{
public:
    std::string name;
    std::string packed_type;
    std::string unpacked_type;
    u64 start;
    u64 stop;
    u64 numOfNeededBitsPacked;
    u64 numOfNeededBitsUnpacked;
    u64 base;

    std::string type_giver(const u64 numOfNeededBits)
    {
        if (numOfNeededBits <= 8) { return "uint8_t"; }
        else if (numOfNeededBits <= 16) { return "uint16_t"; }
        else if (numOfNeededBits <= 32) { return "uint32_t"; }
        else if (numOfNeededBits <= 64) { return "uint64_t"; }
        else { throw std::runtime_error("Too many bits needed - " + std::to_string(numOfNeededBits)); }
    }

    uintPack(std::string name, u64 start,
             u64 stop) // how many numbers we need     <110, 140>   this range needs only 30 numbers -> how many bits do we really need
    {
        CRASH_ON_FALSE(start < stop);

        this->name = name;
        this->start = start;
        this->stop = stop;

        base = start;

        numOfNeededBitsUnpacked = std::bit_width(stop);

        u64 range = stop - start; // 140 - 110 == 30
        numOfNeededBitsPacked = std::bit_width(range);

        var(numOfNeededBitsPacked);
        var(numOfNeededBitsUnpacked);

        unpacked_type = type_giver(numOfNeededBitsUnpacked);
        packed_type = type_giver(numOfNeededBitsPacked);
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
        uintPacks.emplace_back("var_a", 100'000, 100'010);
        uintPacks.emplace_back("var_b", 100, 103);
        uintPacks.emplace_back("var_c", 100, 103);
        uintPacks.emplace_back("var_d", 0, 100'000'000);
        uintPacks.emplace_back("var_e", 100, 103);
        uintPacks.emplace_back("var_f", 100, 103);
        uintPacks.emplace_back("var_g", 100, 103);

        auto totalNumOfNeededBits =
            std::accumulate(uintPacks.begin(), uintPacks.end(), 0, [](u64 sum, const uintPack& pack) { return sum + pack.numOfNeededBitsPacked; });
        numOfNeededBytes = (totalNumOfNeededBits + 7) / 8;

        // reordering Packs //

        // sort by number of needed bits descending
        std::sort(uintPacks.begin(), uintPacks.end(),
                  [](const uintPack& a, const uintPack& b) { return a.numOfNeededBitsPacked > b.numOfNeededBitsPacked; });
    }

    void generate()
    {
// clang-format off
        #define f _file << "\n";
        #define fil(x) _file << x << "\n";
        #define fill(x) _file << x << " ";
        #define file(x) _file << prefix << x << "\n";
        #define filee(x) _file << prefix << x << " ";

        std::ofstream _file("output/test.hpp");
        std::string prefix = "";

        file("#pragma once");
        file("#include <span>");
        file("#include <array>");
        f;
        file("#include \"static_itf_files.h\"");
        f;
        file("class " << name);
        file("{");
        {
            scopedTab tab_1(prefix);
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
            file("char* payload = nullptr;");
            file("const std::size_t known_payload_size = " << numOfNeededBytes << ";");
            f;

            fil("public:");
            f;

            // constructor //
            {
                file(name << "( char* _payload, std::size_t _payload_size )");
                file("{");
                {
                    scopedTab tab_2(prefix);
                    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
                    file("if ( _payload_size != known_payload_size ){");
                    file("\tthrow std::runtime_error(\"Incorrect payload size\");");
                    file("}");
                    f;
                    file("if ( _payload == nullptr ){");
                    file("\tthrow std::runtime_error(\"Payload is nullptr\");");
                    file("}");
                    f;
                    file("payload = _payload;");
                }
                file("}");
                f;


                file(name << "( std::span<char> _payload )");
                file("\t: msg1(_payload.data(), _payload.size())");
                file("{");
                file("}");
                f;
            }

            // packin / unpackin   functions for every member //
            {
                std::size_t current_byte_offset = 0;
                std::size_t acc_bit_offset = 0;

                for (auto& pack : uintPacks)
                {
                    file("explicit void save_" << pack.name << "( const " << pack.unpacked_type << " value )");
                    file("{");
                    {
                        scopedTab tab_2(prefix);
                        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
                        file("CRASH_ON_FALSE(" << pack.start << " <= value " << "&&" << " value <= " << pack.stop << " );");

                        file("packin<" << pack.numOfNeededBitsPacked << ">(" << "static_cast<" << pack.packed_type << ">(value - " << pack.base << "), "
                            << "( payload + " << current_byte_offset << " ), " << acc_bit_offset << ");");
                    }
                    file("}");
                    f;

                    file("" << pack.unpacked_type << " get_" << pack.name << "() const");
                    file("{");
                    {
                        scopedTab tab_2(prefix);
                        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
                        file("return unpackin<" << pack.numOfNeededBitsPacked << ">(( payload + " << current_byte_offset << " ), " << acc_bit_offset << ")");
                        file("\t\t+ " << pack.base << ";");
                    }
                    file("}");
                    f;

                    acc_bit_offset += pack.numOfNeededBitsPacked;
                    if(acc_bit_offset >= 8)
                    {
                        current_byte_offset += (acc_bit_offset / 8);
                        acc_bit_offset %= 8;
                    }
                }
            }
        }

        file("};");

        _file.close();
        // clang-format on
    }
};

int main()
{
    SingleMsgGenerator g("stup - not currently active");
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
