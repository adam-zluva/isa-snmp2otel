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
#include "udpclient.hpp"
#include "snmpbuilder.hpp"

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

    Utils::log("Running ", APP_NAME, "with arguments:");
    Utils::log(args.toString());
    Utils::logSeparator();

    // Load OIDs from file
    Utils::log("Loading OIDs from file: ", args.oidsFile);
    auto oidsOpt = tryLoadOIDs(args.oidsFile);
    if (!oidsOpt.has_value())
        return 1;
    auto oids = oidsOpt.value();
    Utils::log("Loaded OIDs: ", oids);
    Utils::logSeparator();
    
    // So the program doesn't end unexpectedly and properly cleans up
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Create UDP client
    UDPClient udpClient(args.target, args.port);
    if (!udpClient.connect())
    {
        std::cerr << "Failed to connect to " << args.target << ":" << args.port << '\n';
        return 1;
    }

    // Main loop
    Utils::log("Starting loop with interval=", args.interval);
    Utils::logSeparator();
    uint32_t requestId = 1;
    while (!g_stopRequested)
    {
        Utils::log("Sending SNMP request with requestId=", requestId);
        std::vector<uint8_t> response;
        auto requestPDU = SNMPBuilder::buildSNMPGet(args.community, requestId, oids);
        if (!udpClient.sendAndReceive(requestPDU, response, 1000))
        {
            std::cerr << "Failed to send/receive SNMP request\n";
        }
        else
        {
            Utils::log("Received SNMP response of size ", response.size());
            Utils::printHex(response);
        }

        requestId++;
        
        Utils::logSeparator();
        sleep(args.interval);
    }

    udpClient.disconnect();
    return 0;
}