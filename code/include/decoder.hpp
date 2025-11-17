/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "tags.hpp"

class Decoder
{
public:
    /// @brief Constructor
    /// @param data the encoded data
    Decoder(const std::vector<uint8_t>& data);

    /// @brief Resets the decoder state
    void clear();

    /// @return true if there are remaining bytes to decode
    bool hasRemaining() const;
    /// @return number of remaining bytes
    size_t remaining() const;

    // Read next TLV into tag and value (value contains raw content bytes)
    // Throws std::runtime_error on malformed input or when no data left.

    /// @brief Reads the next TLV (Tag-Length-Value) triplet
    void nextTLV(uint8_t &tag, std::vector<uint8_t>& value);

    /// @brief Reads the next integer value
    /// @return the decoded integer
    uint64_t readInteger();
    /// @brief Reads the next string value
    /// @return the decoded string
    std::string readOctetString();
    /// @brief Reads the next NULL value
    void readNull();
    /// @brief Reads the next OID
    /// @return the decoded OID string
    std::string readOID();

private:
    std::vector<uint8_t> buf;
    size_t pos;
};
