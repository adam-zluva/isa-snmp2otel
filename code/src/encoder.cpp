/// ISA ZS 2025
/// xzluva01

#include <stdexcept>
#include <algorithm>

#include "encoder.hpp"
#include "tags.hpp"
#include "ber.hpp"

Encoder::Encoder()
{
    frames.push_back(Frame{0x00, {}}); // just for base, won't collect this
}

void Encoder::clear()
{
    frames.clear();
    frames.push_back(Frame{0x00, {}});
}

void Encoder::startSequence(uint8_t tag)
{
    frames.push_back(Frame{tag, {}});
}

void Encoder::endSequence()
{
    if (frames.size() <= 1)
        throw std::runtime_error("Encoder: no open sequence to end");

    Frame finished = frames.back();
    frames.pop_back();

    appendTLV(finished.tag, finished.content);
}

void Encoder::writeInteger(uint64_t value)
{
    std::vector<uint8_t> val;
    BER::encodeIntegerValue(value, val);
    appendTLV(TAG_INTEGER, val);
}

void Encoder::writeOctetString(const std::string& s)
{
    std::vector<uint8_t> val(s.begin(), s.end());
    appendTLV(TAG_OCTET_STRING, val);
}

void Encoder::writeNull()
{
    std::vector<uint8_t> empty;
    appendTLV(TAG_NULL, empty);
}

void Encoder::writeOID(const std::string& dotted)
{
    std::vector<uint8_t> val;
    BER::encodeOIDValue(dotted, val);
    appendTLV(TAG_OID, val);
}

const std::vector<uint8_t>& Encoder::data() const
{
    if (frames.size() != 1)
        throw std::runtime_error("Encoder: unfinished sequences remain");
    return frames[0].content;
}

Encoder::Frame& Encoder::current()
{
    return frames.back();
}

void Encoder::appendTLV(uint8_t tag, const std::vector<uint8_t>& value)
{
    Frame& f = current();
    f.content.push_back(tag);
    BER::encodeLength(value.size(), f.content);
    f.content.insert(f.content.end(), value.begin(), value.end());
}
