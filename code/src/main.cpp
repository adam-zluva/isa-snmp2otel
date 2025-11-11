/// ISA ZS 2025
/// xzluva01

#include <iostream>
#include "arguments.hpp"

int main(int argc, char** argv)
{
    Arguments arguments;
    if (!arguments.parse(argc, argv))
    {
        if (arguments.hasTarget == false)
            std::cerr << "Target is required\n";
        
        if (arguments.hasOidsFile == false)
            std::cerr << "OIDs file is required\n";

        if (arguments.hasEndpoint == false)
            std::cerr << "Endpoint is required\n";
            
        return 1;
    }
}