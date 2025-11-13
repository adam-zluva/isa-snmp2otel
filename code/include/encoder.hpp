/// ISA ZS 2025
/// xzluva01

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "tags.hpp"

class Encoder
{
public:
    Encoder();
    void clear();

    void startSequence(uint8_t tag);
    void endSequence();

    void writeInteger(uint64_t value);
    void writeOctetString(const std::string& s);
    void writeNull();
    void writeOID(const std::string& dotted);

    const std::vector<uint8_t>& data() const;

private:
    struct Frame {
        uint8_t tag;
        std::vector<uint8_t> content;
    };

    std::vector<Frame> frames; // acts as a stack to handle sequences

    Frame& current();

    void appendTLV(uint8_t tag, const std::vector<uint8_t>& value);
};
