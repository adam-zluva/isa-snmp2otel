/// ISA ZS 2025
/// xzluva01 Adam Žluva

#include <stdexcept>
#include <algorithm>
#include <sstream>
#include "ber.hpp"
#include "tags.hpp"
#include "utils.hpp"

void BER::encodeLength(std::size_t len, std::vector<uint8_t>& out)
{
    if (len < 128)
    {
        out.push_back(static_cast<uint8_t>(len));
        return;
    }

    // len > 127...
    uint8_t buf[8];
    int n = 0;
    while (len > 0)
    {
        buf[n++] = static_cast<uint8_t>(len & 0xFF);
        len >>= 8;
    }

    out.push_back(static_cast<uint8_t>(0x80 | n));
    for (int i = n - 1; i >= 0; --i)
    {
        out.push_back(buf[i]);
    }
}

std::size_t BER::parseLength(const std::vector<uint8_t>& b, size_t &p)
{
    if (p >= b.size())
        throw std::runtime_error("BER: truncated length");

    uint8_t first = b[p++];
    if ((first & 0x80) == 0)
    {
        return first;
    }

    uint8_t n = first & 0x7F;
    if (n == 0)
        throw std::runtime_error("BER: indefinite lengths not supported");
    if (n > 8)
        throw std::runtime_error("BER: length too large");
    if (p + n > b.size())
        throw std::runtime_error("BER: truncated length bytes");

    std::size_t len = 0;
    for (uint8_t i = 0; i < n; ++i)
    {
        len = (len << 8) | static_cast<std::size_t>(b[p++]);
    }
    return len;
}

void BER::encodeIntegerValue(uint64_t v, std::vector<uint8_t>& out)
{
    std::vector<uint8_t> tmp;
    do
    {
        tmp.push_back(static_cast<uint8_t>(v & 0xFF));
        v >>= 8;
    } while (v > 0);

    std::reverse(tmp.begin(), tmp.end());

    if (!tmp.empty() && (tmp[0] & 0x80) != 0)
    {
        tmp.insert(tmp.begin(), 0x00);
    }

    out.insert(out.end(), tmp.begin(), tmp.end());
}

uint64_t BER::decodeIntegerValue(const std::vector<uint8_t>& v)
{
    if (v.empty())
        throw std::runtime_error("BER: empty integer");

    if (v.size() > 9) // 9 bytes is not supported
        throw std::runtime_error("BER: integer too large");

    uint64_t out = 0;
    for (uint8_t b : v)
    {
        out = (out << 8) | static_cast<uint64_t>(b);
    }
    return out;
}

void BER::encodeOIDValue(const std::string& dotted, std::vector<uint8_t>& out)
{
    std::vector<uint32_t> arcs;
    std::string token;
    std::istringstream ss(dotted);

    while (std::getline(ss, token, '.'))
    {
        if (token.empty())
            throw std::runtime_error("Empty arc in OID");
        if (!Utils::isNumeric(token))
            throw std::runtime_error("Invalid OID arc");

        uint32_t val = Utils::atou32(token.c_str());
        arcs.push_back(val);
    }

    if (arcs.size() < 2)
        throw std::runtime_error("OID must have >= 2 arcs");

    uint32_t first = arcs[0];
    uint32_t second = arcs[1];
    if (first > 2 || second > 39)
        throw std::runtime_error("Invalid first two OID arcs");

    out.push_back(static_cast<uint8_t>(40u * first + second));

    // Encode remaining arcs in base-128 (MSB bit set for continuation)
    for (std::size_t i = 2; i < arcs.size(); ++i)
    {
        uint32_t v = arcs[i];
        std::vector<uint8_t> tmp;
        do
        {
            tmp.push_back(static_cast<uint8_t>(v & 0x7F));
            v >>= 7;
        } while (v > 0);

        for (int j = static_cast<int>(tmp.size()) - 1; j >= 0; j--)
        {
            uint8_t byte = tmp[j];
            if (j != 0) byte |= 0x80;
            out.push_back(byte);
        }
    }
}

std::string BER::decodeOIDValue(const std::vector<uint8_t>& v)
{
    if (v.empty())
        throw std::runtime_error("Empty OID");

    std::ostringstream s;

    uint8_t first = v[0];
    uint32_t a = first / 40;
    uint32_t b = first % 40;
    s << a << '.' << b;

    uint32_t acc = 0;
    for (size_t i = 1; i < v.size(); i++)
    {
        uint8_t byte = v[i];
        acc = (acc << 7) | (byte & 0x7F);
        if ((byte & 0x80) == 0)
        {
            s << '.' << acc;
            acc = 0;
        }
    }

    if (acc != 0)
        throw std::runtime_error("Truncated OID base-128 arc");

    return s.str();
}

