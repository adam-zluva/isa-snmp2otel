/// ISA ZS 2025
/// xzluva01 Adam Žluva

#include <iostream>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "udpclient.hpp"
#include "utils.hpp"

UDPClient::UDPClient(const std::string& address, const std::string& port)
    : address(address), port(port), sock(-1), addrInfo(nullptr)
{
}

UDPClient::~UDPClient()
{
    disconnect();
}

bool UDPClient::connect()
{
    // To resolve the address
    struct addrinfo hints {};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(address.c_str(), port.c_str(), &hints, &addrInfo);
    if (rc != 0)
    {
        std::cerr << "getaddrinfo failed: " << gai_strerror(rc) << "\n";
        return false;
    }

    // Socket
    sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        freeaddrinfo(addrInfo);
        addrInfo = nullptr;
        return false;
    }

    Utils::log("UDPClient connected to ", address, ":", port);
    return true;
}

void UDPClient::disconnect()
{
    if (sock >= 0)
    {
        ::close(sock);
        sock = -1;
        Utils::log("UDPClient disconnected from ", address, ":", port);
    }
    if (addrInfo != nullptr)
    {
        freeaddrinfo(addrInfo);
        addrInfo = nullptr;
    }
}

bool UDPClient::sendAndReceive(const std::vector<uint8_t>& request,
                               std::vector<uint8_t>& response,
                               uint32_t timeout)
{
    response.clear();

    if (sock < 0 || addrInfo == nullptr)
    {
        std::cerr << "UDPClient not connected\n";
        return false;
    }

    // tajmaut >:)
    struct timeval tv {};
    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        std::cerr << "setsockopt failed: " << std::strerror(errno) << "\n";
        return false;
    }

    // Send request
    ssize_t sent = sendto(sock, request.data(), request.size(), 0, addrInfo->ai_addr, addrInfo->ai_addrlen);

    if (sent < 0)
    {
        std::cerr << "sendto failed: " << std::strerror(errno) << "\n";
        return false;
    }

    // Receive response
    uint8_t buffer[2048]; // Should be enoguh 
    ssize_t recvd = recvfrom(sock, buffer, sizeof(buffer), 0, nullptr, nullptr);

    if (recvd < 0)
    {
        std::cerr << "recvfrom failed: " 
                  << std::strerror(errno) << "\n";
        return false;
    }

    response.assign(buffer, buffer + recvd);
    return true;
}
