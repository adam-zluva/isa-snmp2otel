/// ISA ZS 2025
/// xzluva01

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <optional>

#include "utils.hpp"
#include "arguments.hpp"
#include "context.hpp"
#include "oidreader.hpp"

const std::string APP_NAME = "snmp2otel";

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

std::optional<std::vector<std::string>> tryLoadOIDs(const std::string& file)
{
    try {
        std::vector<std::string> oids = OIDReader::readOIDsFromFile(file);
        return oids;
    } catch (const std::exception& ex) {
        std::cerr << "Error reading OIDs file: " << ex.what() << '\n';
        return std::nullopt;
    }
}

int main(int argc, char** argv)
{
    // Parse CLI arguments
    auto args = parseArguments(argc, argv);
    if (!args.isValid())
        return 1;

    Context::getInstance(&args); // To initialize Context singleton

    Utils::log(APP_NAME);
    Utils::log(args.toString());

    // Load OIDs from file
    Utils::log("Loading OIDs from file: ", args.oidsFile);
    auto oidsOpt = tryLoadOIDs(args.oidsFile);
    if (!oidsOpt.has_value())
        return 1;
    auto oids = oidsOpt.value();
    Utils::log("Loaded OIDs: ", oids);
    
    // Make sure the program doesn't terminate unexpectedly
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Main loop
    Utils::log("Starting loop with interval=", args.interval);
    while (!g_stopRequested)
    {
        Utils::log("Query...");
        sleep(args.interval);
    }

    Utils::log("\nTerminated");
    return 0;
}