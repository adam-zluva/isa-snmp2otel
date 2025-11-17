/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <string>

#include "tcpclient.hpp"
#include "snmphelper.hpp"

class OTELExporter
{
public:
    OTELExporter(const std::string& endpoint);
    ~OTELExporter();

    bool isValid() const { return valid; }

    /// @brief Connects the client if it isnt
    /// @return true if connected, false otherwise
    bool ensureConnection();

    /// @brief Sends the SNMP values to OTEL endpoint
    /// @param resp SNMP response
    /// @param target the SNMP agent address
    /// @return true on success, false on failure
    bool exportMetrics(const SNMPResponse& resp, const std::string& target, uint32_t timeout);

private:
    /// @brief Parses the OTEL endpoint address
    /// @param endpoint the endpoint address
    /// @return true on success, false on failure
    bool parseEndpoint(const std::string& endpoint);

    std::string endpoint;
    std::string host;
    std::string port;
    std::string path;
    TCPClient client;
    bool valid;
};
