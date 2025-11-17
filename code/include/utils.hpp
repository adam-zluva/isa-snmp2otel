/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <iostream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include "context.hpp"

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        os << v[i];
        if (i + 1 < v.size()) os << ", ";
    }
    os << "]";
    return os;
}

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

    static inline void logSeparator()
    {
        log("----------");
    }

    static inline std::string intToHexString(uint64_t value)
    {
        std::ostringstream s;
        s << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << value;
        return s.str();
    }

    static inline std::string hexVectorToString(const std::vector<uint8_t>& data)
    {
        std::ostringstream s;
        for (std::size_t i = 0; i < data.size(); ++i)
        {
            s << intToHexString(data[i]) << " ";

            // print it out in rows of 16 bytes
            if ((i + 1) % 16 == 0)
                s << "\n";
        }
        return s.str();
    }


    static inline void printHex(const std::vector<uint8_t>& data)
    {
        if (!Context::getInstance().args.verbose) return;
        std::cout << "Hex dump (" << data.size() << " bytes):\n";
        std::cout << hexVectorToString(data) << "\n";
    }

private:
    Utils() {}
};