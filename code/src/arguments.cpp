/// ISA ZS 2025
/// xzluva01

#include <iostream>
#include <unistd.h>
#include "arguments.hpp"
#include "utils.hpp"

#define ARG_OPTSTRING "t:C:o:e:i:r:T:p:v"

/// @brief Asserts that an arg is numeric
/// @param arg The name of the argument
/// @param optarg The argument value
/// @param errFlag Pointer to a bool flag indicating an error
void assertNumeric(std::string arg, std::string optarg, bool* errFlag)
{
    if (!Utils::isNumeric(optarg))
    {
        std::cerr << arg << " requires a number, default will be used" << '\n';
        *errFlag = true;
    }
}

bool Arguments::parse(int argc, char** argv)
{
    int opt;
    opterr = 0; // To silence the default error msg

    while ((opt = getopt(argc, argv, ARG_OPTSTRING)) != -1)
    {
        bool errFlag = false;
        switch (opt)
        {
            case 't':
                this->target = optarg;
                this->hasTarget = true;
                break;

            case 'C':
                this->community = optarg;
                break;

            case 'o':
                this->oidsFile = optarg;
                this->hasOidsFile = true;
                break;

            case 'e':
                this->endpoint = optarg;
                this->hasEndpoint = true;
                break;

            case 'i':
                assertNumeric("-i", optarg, &errFlag);
                if (!errFlag)
                {
                    auto interval = Utils::atou32(optarg);
                    if (interval > 0)
                        this->interval = interval;
                    else
                        std::cerr << "-i must be >0, default will be used" << '\n';
                }
                break;

            case 'r':
                assertNumeric("-r", optarg, &errFlag);
                if (!errFlag)
                    this->retries = Utils::atou32(optarg);
                break;

            case 'T':
                assertNumeric("-T", optarg, &errFlag);
                if (!errFlag)
                    this->timeout = Utils::atou32(optarg);
                break;

            case 'p':
                assertNumeric("-p", optarg, &errFlag);
                if (!errFlag)
                    this->port = optarg;
                break;

            case 'v':
                this->verbose = true;
                break;

            case '?':
            default:
                std::cerr << "Unknown option " << (char)optopt << '\n';
                break;
        }
    }

    return isValid();
}

bool Arguments::isValid() const {
    return this->hasTarget && this->hasOidsFile && this->hasEndpoint;
}

std::string Arguments::invalidMessage() const {
    std::ostringstream s;

    if (!this->hasTarget)
        s << "Target is required\n";
    if (!this->hasOidsFile)
        s << "OIDs file is required\n";
    if (!this->hasEndpoint)
        s << "Endpoint is required\n";

    return s.str();
}

std::string Arguments::toString() const {
    std::ostringstream s;    
    s << "Arguments {" << '\n';
    s << "    target = " << this->target << '\n';
    s << "    community = " << this->community << '\n';
    s << "    oidsFile = " << this->oidsFile << '\n';
    s << "    endpoint = " << this->endpoint << '\n';
    s << "    interval = " << this->interval << '\n';
    s << "    retries = " << this->retries << '\n';
    s << "    timeout = " << this->timeout << '\n';
    s << "    port = " << this->port << '\n';
    s << "    verbose = " << (this->verbose ? "true" : "false") << '\n';
    s << "}";

    return s.str();
}