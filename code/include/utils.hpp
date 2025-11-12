/// ISA ZS 2025
/// xzluva01

#pragma once

#include <cstdlib>
#include <cstdint>
#include "context.hpp"

class Utils
{
public:
    /// @brief Converts a string to an unsigned 32-bit integer
    /// @param s The string to convert
    /// @return The converted uint32
    static inline uint32_t atou32(const char* s) {
        return static_cast<uint32_t>(std::strtoul(s, nullptr, 10));
    }

    /// @brief Checks if a string is numeric
    /// @param string The string to check
    /// @return true if the string is numeric, false otherwise
    static bool isNumeric(const std::string& string)
    {
        for (char c : string)
            if (!std::isdigit(c))
                return false;

        return true;
    }

    /// @brief Logs to stdout if verbose was enabled
    template<typename... Args>
    static inline void log(const Args&... args)
    {
        if (!Context::getInstance().args.verbose) return;
        (std::cout << ... << args) << '\n';
    }

private:
    Utils() {}
};