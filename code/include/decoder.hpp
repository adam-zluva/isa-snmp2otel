/// ISA ZS 2025
/// xzluva01

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "tags.hpp"

class Decoder
{
public:
    Decoder(const std::vector<uint8_t>& data);

    void clear();

    bool hasRemaining() const;
    size_t remaining() const;

    // Read next TLV into tag and value (value contains raw content bytes)
    // Throws std::runtime_error on malformed input or when no data left.
    void nextTLV(uint8_t &tag, std::vector<uint8_t>& value);

    uint64_t readInteger();
    std::string readOctetString();
    void readNull();
    std::string readOID();

    static uint64_t decodeIntegerValue(const std::vector<uint8_t>& v);
    static std::string decodeOIDValue(const std::vector<uint8_t>& v);

private:
    std::vector<uint8_t> buf;
    size_t pos;
};
