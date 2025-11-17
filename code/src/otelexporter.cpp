/// ISA ZS 2025
/// xzluva01 Adam Žluva

#include <iostream>
#include <sstream>
#include <chrono>

#include "otelexporter.hpp"
#include "json.hpp"
#include "utils.hpp"

using json = nlohmann::json; // Library from https://github.com/nlohmann/json

OTELExporter::OTELExporter(const std::string& endpoint)
    : endpoint(endpoint), host(), port(), path(), valid(false)
{
    valid = parseEndpoint(endpoint);
}

OTELExporter::~OTELExporter()
{
    client.disconnect();
}

// too hardcoded ://
bool OTELExporter::parseEndpoint(const std::string& endpoint)
{
    const std::string http = "http://";
    if (endpoint.rfind(http, 0) != 0)
    {
        std::cerr << "Endpoint must start with 'http://': " << endpoint << "\n";
        return false;
    }

    std::string s = endpoint.substr(http.size());

    // split host[:port] and path
    size_t slash = s.find('/');
    std::string hostport = (slash == std::string::npos) ? s : s.substr(0, slash);
    path = (slash == std::string::npos) ? "/" : s.substr(slash);

    // split port
    size_t colon = hostport.find(':');
    if (colon == std::string::npos)
    {
        host = hostport;
        port = "80";
    }
    else
    {
        host = hostport.substr(0, colon);
        port = hostport.substr(colon + 1);
    }

    if (host.empty())
    {
        std::cerr << "Invalid endpoint (empty host): " << endpoint << "\n";
        return false;
    }

    return true;
}

bool OTELExporter::ensureConnection()
{
    return client.connect(host, port);
}

// Don't really understand why this is necessary but other students told me to do this
// and LLMs agreed so...
static std::string sanitizeMetricName(const std::string& oid)
{
    std::string out;
    for (char c : oid)
    {
        if (std::isalnum(static_cast<unsigned char>(c))) out.push_back(c);
        else out.push_back('_');
    }
    return "snmp_" + out;
}

bool OTELExporter::exportMetrics(const SNMPResponse& resp, const std::string& target, uint32_t timeout)
{
    if (!valid)
        return false;

    if (!ensureConnection())
    {
        std::cerr << "Failed to connect to OTEL endpoint" << "\n";
        return false;
    }

    // OTLP/HTTP JSON
    // (resourceMetrics -> scopeMetrics -> metrics)
    json root;
    root["resourceMetrics"] = json::array();

    json resource;
    resource["resource"]["attributes"] = json::array({
        { {"key", "service.name"}, {"value", { {"stringValue", APP_NAME} } } },
        { {"key", "target"}, {"value", { {"stringValue", target} } } }
    });

    resource["scopeMetrics"] = json::array();

    json scopeMetric;
    scopeMetric["scope"] = json::object();
    scopeMetric["metrics"] = json::array();

    // timestamp in nano secodns
    auto now = std::chrono::system_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

    for (const auto& vb : resp.varbinds)
    {
        json metric;
        metric["name"] = sanitizeMetricName(vb.oid);
        metric["description"] = "SNMP metric for OID " + vb.oid;
        metric["unit"] = "nope"; // i dont't have energy to deal with units ://

        metric["gauge"]["dataPoints"] = json::array();
        json dp;
        // Always include oid attribute
        dp["attributes"] = json::array({
            { {"key", "oid"}, {"value", { {"stringValue", vb.oid} } } }
        });
        dp["timeUnixNano"] = std::to_string(ns);

        // Try convert to double - if fails, send 0.0 and keep original value
        double valDouble = 0.0;
        bool ok = true;
        try
        {
            valDouble = std::stod(vb.value);
        } catch (...)
        {
            ok = false;
        }

        if (!ok) // include the original string as an attribute so the value isn't lost
            dp["attributes"].push_back({ {"key", "value"}, {"value", { {"stringValue", vb.value} } } });

        dp["asDouble"] = valDouble;
        metric["gauge"]["dataPoints"].push_back(dp);

        scopeMetric["metrics"].push_back(metric);
    }

    resource["scopeMetrics"].push_back(scopeMetric);
    root["resourceMetrics"].push_back(resource);

    std::string body = root.dump();

    // Build HTTP request
    std::ostringstream req;
    req << "POST " << path << " HTTP/1.1" << "\r\n";
    req << "Host: " << host << "\r\n";
    req << "Content-Type: application/json; charset=utf-8" << "\r\n";
    req << "Content-Length: " << body.size() << "\r\n";
    req << "Connection: close" << "\r\n";
    req << "\r\n";
    req << body;

    std::string requestStr = req.str();

    if (!client.sendAll(requestStr))
    {
        std::cerr << "OTEL exporter failed to send HTTP request" << "\n";
        client.disconnect();
        return false;
    }

    std::string response;
    if (!client.receiveAll(response, timeout))
    {
        std::cerr << "OTEL exproter failed to read HTTP response" << "\n";
        client.disconnect();
        return false;
    }

    Utils::log("OTEL received response:\n", response);
    return true;
}
