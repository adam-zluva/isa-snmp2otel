#include <iostream>
#include <fstream>
#include <algorithm>
#include "oidreader.hpp"

std::vector<std::string> OIDReader::readOIDsFromFile(const std::string& filename)
{
    std::vector<std::string> oids;
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Could not open file: " + filename);
    std::string line;

    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end()); // Remove spaces
        if (line.empty() || line[0] == '#') // Skip empty lines and comments
            continue;

        if (!validateOID(line)) // Skip invalid OIDs
        {
            std::cerr << "Invalid OID found: " << line << '\n';
            continue;
        }

        oids.push_back(line);
    }

    return oids;
}

bool OIDReader::validateOID(const std::string& oid)
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