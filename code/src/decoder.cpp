/// ISA ZS 2025
/// xzluva01

#include <stdexcept>
#include <sstream>
#include <algorithm>

#include "decoder.hpp"
#include "ber.hpp"

Decoder::Decoder(const std::vector<uint8_t>& data): buf(data), pos(0) { }

void Decoder::clear()
{
    buf.clear();
    pos = 0;
}

bool Decoder::hasRemaining() const
{
    return pos < buf.size();
}

size_t Decoder::remaining() const
{
    return (pos < buf.size()) ? (buf.size() - pos) : 0;
}

void Decoder::nextTLV(uint8_t &tag, std::vector<uint8_t>& value)
{
    if (pos >= buf.size())
        throw std::runtime_error("Decoder: no more data");

    tag = buf[pos++];

    size_t len = BER::parseLength(buf, pos);

    if (pos + len > buf.size())
        throw std::runtime_error("Decoder: truncated content");

    value.assign(buf.begin() + pos, buf.begin() + pos + len);
    pos += len;
}

uint64_t Decoder::decodeIntegerValue(const std::vector<uint8_t>& v)
{
    return BER::decodeIntegerValue(v);
}

uint64_t Decoder::readInteger()
{
    uint8_t tag;
    std::vector<uint8_t> val;
    nextTLV(tag, val);
    if (tag != TAG_INTEGER)
        throw std::runtime_error("Decoder: expected INTEGER tag");
    return BER::decodeIntegerValue(val);
}

std::string Decoder::readOctetString()
{
    uint8_t tag;
    std::vector<uint8_t> val;
    nextTLV(tag, val);
    if (tag != TAG_OCTET_STRING)
        throw std::runtime_error("Decoder: expected OCTET STRING tag");
    return std::string(val.begin(), val.end());
}

void Decoder::readNull()
{
    uint8_t tag;
    std::vector<uint8_t> val;
    nextTLV(tag, val);
    if (tag != TAG_NULL)
        throw std::runtime_error("Decoder: expected NULL tag");
    if (!val.empty())
        throw std::runtime_error("Decoder: NULL with non-empty content");
}

std::string Decoder::decodeOIDValue(const std::vector<uint8_t>& v)
{
    return BER::decodeOIDValue(v);
}

std::string Decoder::readOID()
{
    uint8_t tag;
    std::vector<uint8_t> val;
    nextTLV(tag, val);
    if (tag != TAG_OID)
        throw std::runtime_error("Decoder: expected OID tag");
    return BER::decodeOIDValue(val);
}
