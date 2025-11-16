#pragma once

#include <string>
#include "tcpclient.hpp"
#include "snmphelper.hpp"

class OTELExporter
{
public:
    OTELExporter(const std::string& endpoint);
    ~OTELExporter();

    bool isValid() const;

    // Export metrics for a single SNMPResponse. `target` is the SNMP device address
    bool exportMetrics(const SNMPResponse& resp, const std::string& target);

private:
    bool parseEndpoint(const std::string& endpoint);
    bool ensureConnected();

    std::string endpoint;
    std::string host;
    std::string port;
    std::string path;
    TCPClient client;
    bool valid;
};
