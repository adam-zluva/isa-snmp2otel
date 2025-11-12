/// ISA ZS 2025
/// xzluva01

#pragma once
#include <string>
#include <sstream>
#include <cstdint>

struct Arguments
{
    bool hasTarget;
    std::string target;
    std::string community;
    bool hasOidsFile;
    std::string oidsFile;
    bool hasEndpoint;
    std::string endpoint;
    uint32_t interval;
    uint32_t retries;
    uint32_t timeout;
    std::string port;
    bool verbose;

    Arguments()
    {
        hasTarget = false;
        hasOidsFile = false;
        hasEndpoint = false;
        verbose = false;

        community = "public";
        interval = 10;
        retries = 2;
        timeout = 1000;
        port = "161";
    }

    /// @brief Initializes the Arguments struct by parsing command line arguments
    /// @return true if parsing was successful, false otherwise
    bool parse(int argc, char** argv);

    /// @brief Checks if the args are valid
    /// @return true if the arguments are valid, false otherwise
    bool isValid() const;

    /// @brief Returns the reason why the arguments are invalid
    /// @return A string describing the error
    std::string invalidMessage() const;

    std::string toString() const;
};
