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

    bool parse(int argc, char** argv);

    std::string toString() const;
};
