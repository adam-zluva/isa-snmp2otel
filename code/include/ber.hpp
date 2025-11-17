/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <cstdint>
#include <vector>
#include <string>

class BER
{
public:
    static void encodeLength(std::size_t len, std::vector<uint8_t>& out);
    static std::size_t parseLength(const std::vector<uint8_t>& b, size_t &p);

    static void encodeIntegerValue(uint64_t v, std::vector<uint8_t>& out);
    static uint64_t decodeIntegerValue(const std::vector<uint8_t>& v);

    static void encodeOIDValue(const std::string& dotted, std::vector<uint8_t>& out);
    static std::string decodeOIDValue(const std::vector<uint8_t>& v);
};
