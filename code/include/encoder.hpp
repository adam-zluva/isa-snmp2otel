/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "tags.hpp"

class Encoder
{
public:
    Encoder();

    /// @brief Resets the encoder state
    void clear();

    /// @brief Starts a new sequence
    /// @param tag the sequence tag
    void startSequence(uint8_t tag);
    /// @brief Ends the current sequence
    void endSequence();

    /// @brief Encodes an integer value
    /// @param value the integer to encode
    void writeInteger(uint64_t value);
    /// @brief Encodes a string
    /// @param s the string to encode
    void writeOctetString(const std::string& s);
    /// @brief Encodes a NULL value
    void writeNull();
    /// @brief Encodes an OID
    /// @param dotted the OID string to encode
    void writeOID(const std::string& dotted);

    /// @return the encoded byte vector
    const std::vector<uint8_t>& data() const;

private:
    /// @brief Helper struct that acts as a frame for sequences
    struct Frame {
        uint8_t tag;
        std::vector<uint8_t> content;
    };

    std::vector<Frame> frames; // acts as a stack to handle sequences

    /// @return reference to the current frame
    Frame& current();

    void appendTLV(uint8_t tag, const std::vector<uint8_t>& value);
};
