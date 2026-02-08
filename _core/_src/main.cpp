#include <fstream>
#include <iostream>

using namespace std;

#include "json.hpp" // external lib
#include "math.h"   // internal lib
#include "util.h"   // local

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
    u64 default_value;

    std::string type_giver(const u64 numOfNeededBits)
    {
        if (numOfNeededBits <= 8) { return "uint8_t"; }
        else if (numOfNeededBits <= 16) { return "uint16_t"; }
        else if (numOfNeededBits <= 32) { return "uint32_t"; }
        else if (numOfNeededBits <= 64) { return "uint64_t"; }
        else { throw std::runtime_error("Too many bits needed - " + std::to_string(numOfNeededBits)); }
    }

    uintPack(std::string name, u64 start, u64 stop)
    {
        CRASH_ON_FALSE(start < stop);

        this->name = name;
        this->start = start;
        this->stop = stop;
        this->default_value = start;

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
        uintPacks.emplace_back("var_a", 0, 100'000'000);
        uintPacks.emplace_back("var_b", 100'000, 100'010);
        uintPacks.emplace_back("var_c", 0, 10);
        uintPacks.emplace_back("var_d", 0, 10);
        uintPacks.emplace_back("var_e", 0, 10);
        uintPacks.emplace_back("var_f", 0, 10);
        uintPacks.emplace_back("var_g", 0, 10);

        auto totalNumOfNeededBits =
            std::accumulate(uintPacks.begin(), uintPacks.end(), 0, [](u64 sum, const uintPack& pack) { return sum + pack.numOfNeededBitsPacked; });
        numOfNeededBytes = (totalNumOfNeededBits + 7) / 8;

        // reordering Packs //

        // sort by number of needed bits descending
        std::sort(uintPacks.begin(), uintPacks.end(),
                  [](const uintPack& a, const uintPack& b) { return a.numOfNeededBitsPacked > b.numOfNeededBitsPacked; });
    }

    // clang-format off
    void generate()
    {
        #define f _file << "\n";
        #define fil(x) _file << x << "\n";
        #define filll(x) _file << x << " ";
        #define file(x) _file << prefix << x << "\n";
        #define filee(x) _file << prefix << x << " ";

        std::ofstream _file("output/test.hpp");
        std::string prefix = "";

        file("#pragma once");
        f;
        file("#include \"static_itf_files.h\"");
        f;
        file("class " << name);
        file("{");
        {
            scopedTab tab_1(prefix);
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
            file("char* payload = nullptr;");
            f;

            file("public:");
            f;
            file("static constexpr std::size_t known_payload_size = " << numOfNeededBytes << ";");
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
                    f;

                    // default values for every member //
                    for (auto& pack : uintPacks)
                    {
                        file("save_" << pack.name << "( " << pack.default_value << " );");
                    }
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
                    file("void save_" << pack.name << "( const " << pack.unpacked_type << " value )");
                    file("{");
                    {
                        scopedTab tab_2(prefix);
                        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
                        file("CRASH_ON_FALSE( " << pack.start << " <= value " << "&&" << " value <= " << pack.stop << " );");

                        file("packin<" << pack.packed_type << ", " << pack.numOfNeededBitsPacked << ">(" << "static_cast<" << pack.packed_type << ">(value - " << pack.base << "), "
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
    }
    // clang-format on
};

#include "test.hpp"

int main()
{
    SingleMsgGenerator g("stup - not currently active");
    g.generate();

    vector<char> payload(msg1::known_payload_size);

    msg1 msg(payload);
    // msg.save_var_a(100'000'000);
    // msg.save_var_b(100'000);
    // msg.save_var_c(1);
    // msg.save_var_d(2);
    // msg.save_var_e(3);
    // msg.save_var_f(4);
    // msg.save_var_g(10);

    var(msg.get_var_a());
    var(msg.get_var_b());
    var(msg.get_var_c());
    var(msg.get_var_d());
    var(msg.get_var_e());
    var(msg.get_var_f());
    var(msg.get_var_g());

    return 0;
}

// int main()
// {
//     SingleMsgGenerator g("stup - not currently active");
//     g.generate();
// }

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
