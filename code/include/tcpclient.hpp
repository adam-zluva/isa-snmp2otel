/// ISA ZS 2025
/// xzluva01

#pragma once

#include <string>
#include <sys/types.h>

class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    bool connect(const std::string& host, const std::string& port);
    void disconnect();

    // send entire buffer, returns false on failure
    bool sendAll(const std::string& data);

    // receive until remote close or timeout (ms). Returns false on error.
    bool receiveAll(std::string& out, int timeoutMs = 2000);

private:
    int sock;
    struct addrinfo* addrInfo;
    std::string host;
    std::string port;
};
