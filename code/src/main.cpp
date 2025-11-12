/// ISA ZS 2025
/// xzluva01

#include <iostream>
#include "utils.hpp"
#include "arguments.hpp"
#include "context.hpp"

Arguments parseArguments(int argc, char** argv)
{
    Arguments arguments;
    if (!arguments.parse(argc, argv))
        std::cerr << "Error parsing arguments:\n" << arguments.invalidMessage();

    return arguments;
}

int main(int argc, char** argv)
{
    Arguments args = parseArguments(argc, argv);
    if (!args.isValid())
        return 1;

    Context::getInstance(&args); // to initialize Context singleton

    
    return 0;
}