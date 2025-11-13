#pragma once

#include <vector>
#include <string>
#include <cstdint>


class SNMPBuilder
{
public:
    static std::vector<uint8_t> buildSNMPGet(const std::string& community, uint32_t requestId, const std::vector<std::string>& oids);
};