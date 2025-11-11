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
    if (!isNumeric(optarg))
    {
        std::cerr << arg << " requires a number" << '\n';
        *errFlag = true;
    }
}

/// @brief Initializes the Arguments struct by parsing command line arguments
/// @return true if parsing was successful, false otherwise
bool Arguments::parse(int argc, char** argv)
{
    int opt;
    opterr = 0; // To silence the default error msg

    bool errFlag = false;

    while ((opt = getopt(argc, argv, ARG_OPTSTRING)) != -1)
    {
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
                std::cout << "bruh";
                break;

            case 'i':
                assertNumeric("-i", optarg, &errFlag);
                this->interval = atou32(optarg);
                break;

            case 'r':
                assertNumeric("-r", optarg, &errFlag);
                this->retries = atou32(optarg);
                break;

            case 'T':
                assertNumeric("-T", optarg, &errFlag);
                this->timeout = atou32(optarg);
                break;

            case 'p':
                assertNumeric("-p", optarg, &errFlag);
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

    return !errFlag && this->hasTarget && this->hasOidsFile && this->hasEndpoint;
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