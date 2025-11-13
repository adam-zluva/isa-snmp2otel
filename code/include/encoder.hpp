/// ISA ZS 2025
/// xzluva01

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#define TAG_SEQUENCE        0x30
#define TAG_INTEGER         0x02
#define TAG_OCTET_STRING    0x04
#define TAG_NULL            0x05
#define TAG_OID             0x06
#define TAG_GET_RQ_PDU      0xA0

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

    static void encodeLength(std::size_t len, std::vector<uint8_t>& out);
    static void encodeIntegerValue(uint64_t v, std::vector<uint8_t>& out);
    static void encodeOIDValue(const std::string& dotted, std::vector<uint8_t>& out);

    void appendTLV(uint8_t tag, const std::vector<uint8_t>& value);
};
