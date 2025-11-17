/// ISA ZS 2025
/// xzluva01 Adam Žluva

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <optional>

#include "utils.hpp"
#include "arguments.hpp"
#include "context.hpp"
#include "oidreader.hpp"
#include "udpclient.hpp"
#include "snmphelper.hpp"
#include "otelexporter.hpp"

const std::string APP_NAME = "snmp2otel";

static bool g_stopRequested = false; // signals to stop the send loop

/// @brief Signal handler to intercept ctrl+c signals
static void signalHandler(int)
{
    g_stopRequested = true;
}

/// @brief Helper function to parse cli args
/// @return Arguments struct containing the parsed arguments
Arguments parseArguments(int argc, char** argv)
{
    Arguments arguments;
    if (!arguments.parse(argc, argv))
        std::cerr << "Error parsing arguments:\n" << arguments.invalidMessage();

    return arguments;
}

/// @brief Helper function to load OIDS from file
/// @param file path to the OIDs file
/// @return Vector of filtered OIDs on success, otherwise nullopt
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

    // To be fancy, because I like being extra
    Utils::log("Running ", APP_NAME, " with arguments:");
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

    // Create UDP client for SNMP
    UDPClient udpClient(args.target, args.port);
    if (!udpClient.connect())
        return 1;

    // Create OTEL exporter
    OTELExporter exporter(args.endpoint);
    if (!exporter.isValid())
    {
        std::cerr << "Invalid OTEL endpoint: " << args.endpoint << '\n';
        return 1;
    }

    // Main loop
    Utils::log("Starting loop with interval=", args.interval);
    Utils::logSeparator();
    uint32_t requestId = 0;
    size_t attempt = 0;
    while (!g_stopRequested && attempt <  args.retries)
    {
        auto requestPDU = SNMPHelper::buildSNMPGet(args.community, requestId++, oids);
        std::vector<uint8_t> response;
        Utils::log("Sending SNMP request with requestId=", requestId);
        if (!udpClient.sendAndReceive(requestPDU, response, args.timeout))
        {
            attempt++;
            std::cerr << "Failed to send/receive SNMP request" << "\n";

            if (attempt >= args.retries)
            {
                Utils::log("Failed to reach target in ", args.retries, " retries");
            }
        }
        else
        {
            attempt = 0;
            Utils::log("Received SNMP response of size ", response.size());

            auto snmpResponse = SNMPHelper::decodeResponse(response);
            Utils::log("Decoded SNMP Response:\n", snmpResponse.toString());

            if (!exporter.exportMetrics(snmpResponse, args.target))
            {
                std::cerr << "Failed to export metrics to OTEL endpoint" << "\n";
            }
        }

        if (!g_stopRequested && attempt < args.retries)
            sleep(args.interval);
    }

    return 0;
}