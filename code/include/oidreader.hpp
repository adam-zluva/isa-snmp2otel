/// ISA ZS 2025
/// xzluva01

#pragma once

#include <string>
#include <vector>
#include <fstream>

class OIDReader
{
public:
    /// @brief Reads all lines from a text file and stores them in a vector
    /// @param filename The name of the file to read
    /// @return A vector containing all lines from the file
    static std::vector<std::string> readOIDsFromFile(const std::string& filename)
    {
        std::vector<std::string> oids;
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line))
        {
            oids.push_back(line);
        }

        return oids;
    }

    /// @brief Checks if a given OID string is valid
    /// @param oid The OID to validate
    /// @return true if the OID is valid, false otherwise
    static bool validateOID(const std::string& oid)
    {
        if (oid.empty())
            return false;

        for (char c : oid)
        {
            if (!std::isdigit(c) && c != '.')
                return false;
        }

        return true;
    }
};