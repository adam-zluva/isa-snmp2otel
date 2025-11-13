/// ISA ZS 2025
/// xzluva01

#include <stdexcept>
#include <algorithm>

#include "encoder.hpp"

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
    if (frames.size() <= 1) {
        throw std::runtime_error("Encoder: no open sequence to end");
    }

    Frame finished = frames.back();
    frames.pop_back();

    appendTLV(finished.tag, finished.content);
}

void Encoder::writeInteger(uint64_t value)
{
    std::vector<uint8_t> val;
    encodeIntegerValue(value, val);
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
    encodeOIDValue(dotted, val);
    appendTLV(TAG_OID, val);
}

const std::vector<uint8_t>& Encoder::data() const
{
    if (frames.size() != 1) {
        throw std::runtime_error("Encoder: unfinished sequences remain");
    }
    return frames[0].content;
}

Encoder::Frame& Encoder::current()
{
    return frames.back();
}

void Encoder::encodeLength(std::size_t len, std::vector<uint8_t>& out)
{
    if (len < 128) {
        out.push_back(static_cast<uint8_t>(len));
        return;
    }

    // len > 127...
    uint8_t buf[8];
    int n = 0;
    while (len > 0) {
        buf[n++] = static_cast<uint8_t>(len & 0xFF);
        len >>= 8;
    }

    out.push_back(static_cast<uint8_t>(0x80 | n));
    for (int i = n - 1; i >= 0; --i) {
        out.push_back(buf[i]);
    }
}

void Encoder::encodeIntegerValue(uint64_t v, std::vector<uint8_t>& out)
{
    // rewrite this later, this is ugly

    std::vector<uint8_t> tmp;
    do {
        tmp.push_back(static_cast<uint8_t>(v & 0xFF));
        v >>= 8;
    } while (v > 0);

    std::reverse(tmp.begin(), tmp.end());

    if (!tmp.empty() && (tmp[0] & 0x80) != 0) {
        tmp.insert(tmp.begin(), 0x00);
    }

    out.insert(out.end(), tmp.begin(), tmp.end());
}

void Encoder::encodeOIDValue(const std::string& dotted, std::vector<uint8_t>& out)
{
    // parse arcs
    std::vector<uint32_t> arcs;
    uint64_t acc = 0;
    bool inNumber = false;

    for (char c : dotted) {
        if (c == '.') {
            if (!inNumber)
                throw std::runtime_error("Encoder: empty arc in OID");
            arcs.push_back(static_cast<uint32_t>(acc));
            acc = 0;
            inNumber = false;
        } else if (c >= '0' && c <= '9') {
            inNumber = true;
            acc = acc * 10 + static_cast<uint32_t>(c - '0');
            if (acc > 0xFFFFFFFFull)
                throw std::runtime_error("Encoder: OID arc too large");
        } else {
            throw std::runtime_error("Encoder: invalid OID");
        }
    }
    if (inNumber)
        arcs.push_back(static_cast<uint32_t>(acc));

    if (arcs.size() < 2) {
        throw std::runtime_error("Encoder: OID must have ≥ 2 arcs");
    }

    // first byte
    uint32_t first = arcs[0];
    uint32_t second = arcs[1];

    if (first > 2 || second > 39)
        throw std::runtime_error("Encoder: invalid first two OID arcs");

    out.push_back(static_cast<uint8_t>(40u * first + second));

    // remaining arcs in base-128
    for (std::size_t i = 2; i < arcs.size(); ++i) {
        uint32_t v = arcs[i];
        uint8_t tmp[5];
        int n = 0;

        do {
            tmp[n++] = static_cast<uint8_t>(v & 0x7F);
            v >>= 7;
        } while (v > 0);

        for (int j = n - 1; j >= 0; --j) {
            uint8_t byte = tmp[j];
            if (j != 0)
                byte |= 0x80;
            out.push_back(byte);
        }
    }
}

void Encoder::appendTLV(uint8_t tag, const std::vector<uint8_t>& value)
{
    Frame& f = current();
    f.content.push_back(tag);
    encodeLength(value.size(), f.content);
    f.content.insert(f.content.end(), value.begin(), value.end());
}
