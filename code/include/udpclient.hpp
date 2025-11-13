/// ISA ZS 2025
/// xzluva01

#pragma once

#include <string>
#include <vector>
#include <cstdint>

class UDPClient
{
public:
    UDPClient(const std::string& address, const std::string& port);
    ~UDPClient();

    /// @brief Creates a UDP connection to the specified address and port
    /// @return true on success, false on failure
    bool connect();

    /// @brief Disconnects the UDP connection
    void disconnect();

    /// @brief Sends a UDP request and waits for a response
    /// @param request The data to send
    /// @param response The received data
    /// @param timeoutMs Timeout in milliseconds
    /// @return true on success, false on failure or timeout
    bool sendAndReceive(const std::vector<uint8_t>& request,
                        std::vector<uint8_t>& response,
                        uint32_t timeout);

private:
    std::string address;
    std::string port;

    int sock;
    struct addrinfo* addrInfo;
};
