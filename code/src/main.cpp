/// ISA ZS 2025
/// xzluva01

#include <iostream>
#include <csignal>

#include "utils.hpp"
#include "arguments.hpp"
#include "context.hpp"

static bool g_stopRequested = false;
static void signalHandler(int)
{
    g_stopRequested = true;
}

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

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler); // to make sure the program doesn't terminate unexpectedly

    while (!g_stopRequested)
    {
        continue;
    }

    Utils::log("\nTerminated");
    return 0;
}