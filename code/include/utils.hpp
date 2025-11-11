/// ISA ZS 2025
/// xzluva01

#pragma once

#include <cstdlib>
#include <cstdint>

/// @brief Converts a string to an unsigned 32-bit integer
/// @param s The string to convert
/// @return The converted uint32
inline uint32_t atou32(const char* s) {
    return static_cast<uint32_t>(std::strtoul(s, nullptr, 10));
}

/// @brief Checks if a string is numeric
/// @param string The string to check
/// @return true if the string is numeric, false otherwise
bool isNumeric(const std::string& string)
{
    for (char c : string)
        if (!std::isdigit(c))
            return false;

    return true;
}