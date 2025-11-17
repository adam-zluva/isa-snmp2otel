/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <vector>
#include <string>
#include <cstdint>


// A single variable binding from a response
struct SNMPVarBind
{
    std::string oid;
    uint8_t tag;
    std::string value; // string representation of value

    std::string toString() const;
};

// Decoded SNMP response
struct SNMPResponse {
    uint64_t requestId;
    uint64_t errorStatus;
    uint64_t errorIndex;
    std::vector<SNMPVarBind> varbinds;

    std::string toString() const;
};

class SNMPHelper
{
public:
    static std::vector<uint8_t> buildSNMPGet(const std::string& community, uint32_t requestId, const std::vector<std::string>& oids);
    static SNMPResponse decodeResponse(const std::vector<uint8_t>& data);
};