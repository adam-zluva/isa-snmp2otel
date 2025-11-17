/// ISA ZS 2025
/// xzluva01 Adam Žluva

#pragma once

#include <string>
#include <sys/types.h>

class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    /// @brief Connects to the specified host and port
    /// @param host the host address
    /// @param port the port
    /// @return true on success, false otherwise
    bool connect(const std::string& host, const std::string& port);

    /// @brief Closes the connection
    void disconnect();

    /// @return true if connected, false otherwise
    bool isConnected() const { return sock >= 0 && addrInfo != nullptr; }

    /// @brief Sends the data
    /// @param data the data to send
    /// @return true on success, false otherwise
    bool sendAll(const std::string& data);

    /// @brief Receives data
    /// @param out the received data
    /// @param timeout timeout in milliseconds
    /// @return true on success, false on failure or timeout
    bool receiveAll(std::string& out, uint32_t timeout);

private:
    int sock;
    struct addrinfo* addrInfo;
    std::string host;
    std::string port;
};
