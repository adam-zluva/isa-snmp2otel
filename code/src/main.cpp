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
#include "snmphelper.hpp"
#include "otel_exporter.hpp"

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
    // Filter to scalar OIDs ending with .0 as required
    std::vector<std::string> scalarOids;
    for (const auto &o : oids)
    {
        if (o.size() >= 2 && o.rfind(".0") == o.size() - 2)
            scalarOids.push_back(o);
        else
            Utils::log("Ignoring non-scalar OID (not ending with .0): ", o);
    }

    if (scalarOids.empty())
    {
        std::cerr << "No scalar OIDs (ending with .0) to poll.\n";
        return 1;
    }
    oids = scalarOids;
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

    // Create OTEL exporter on stack (std::optional) if endpoint provided
    std::optional<OTELExporter> exporter;
    if (args.hasEndpoint)
    {
        exporter.emplace(args.endpoint);
        if (!exporter->isValid())
        {
            std::cerr << "Invalid OTEL endpoint: " << args.endpoint << '\n';
            exporter.reset();
        }
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

                if (exporter.has_value())
                {
                    if (!exporter->exportMetrics(snmpResponse, args.target))
                    {
                        std::cerr << "Failed to export metrics to OTEL endpoint" << "\n";
                    }
                }
            }

        if (!g_stopRequested && attempt < args.retries)
            sleep(args.interval);
    }

    return 0;
}