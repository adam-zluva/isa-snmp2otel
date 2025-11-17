/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <cstdint>
#include <vector>
#include <string>

class BER
{
public:
    /// @brief encodes length in BER format
    /// @param len the length
    /// @param out the encoded bytes
    static void encodeLength(std::size_t len, std::vector<uint8_t>& out);

    // @brief parses length in BER format
    /// @param b encoded bytes
    /// @param p position to start reading from
    /// @return the decoded length
    static std::size_t parseLength(const std::vector<uint8_t>& b, size_t &p);

    /// @brief encodes integer value in BER format
    /// @param v the integer
    /// @param out the encoded bytes
    static void encodeIntegerValue(uint64_t v, std::vector<uint8_t>& out);

    /// @brief decodes integer in BER format
    /// @param v the encoded bytes
    /// @return the decoded integer
    static uint64_t decodeIntegerValue(const std::vector<uint8_t>& v);

    /// @brief encodes OID value in BER format
    /// @param dotted the dotted OID string
    /// @param out the encoded bytes
    static void encodeOIDValue(const std::string& dotted, std::vector<uint8_t>& out);

    /// @brief decodes OID in BER format
    /// @param v encoded bytes
    /// @return the decoded OID string
    static std::string decodeOIDValue(const std::vector<uint8_t>& v);
};
