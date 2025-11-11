#include "oidreader.hpp"

std::vector<std::string> OIDReader::readOIDsFromFile(const std::string& filename)
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