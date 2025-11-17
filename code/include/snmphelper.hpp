/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <vector>
#include <string>
#include <cstdint>


/// @brief Holds SNMP metric data
struct SNMPVarBind
{
    std::string oid;
    uint8_t tag;
    std::string value; // string representation of value

    std::string toString() const;
};

/// @brief Representation of the SNMP response
struct SNMPResponse
{
    uint64_t requestId;
    uint64_t errorStatus;
    uint64_t errorIndex;
    std::vector<SNMPVarBind> varbinds;

    std::string toString() const;
};

class SNMPHelper
{
public:
    /// @brief Builds an SNMP GET request
    /// @param community the community string
    /// @param requestId the request ID
    /// @param oids OIDs to request
    /// @return the encoded SNMP GET request
    static std::vector<uint8_t> buildSNMPGet(const std::string& community, uint32_t requestId, const std::vector<std::string>& oids);

    /// @brief Decodes an SNMP response
    /// @param data the encoded SNMP response
    /// @return the decoded SNMP data as SNMPResponse
    static SNMPResponse decodeResponse(const std::vector<uint8_t>& data);
};