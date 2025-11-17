/// ISA ZS 2025
/// xzluva01 Adam Žluva

#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include "tcpclient.hpp"
#include "utils.hpp"

TCPClient::TCPClient()
    : sock(-1), addrInfo(nullptr)
{
}

TCPClient::~TCPClient()
{
    disconnect();
}

bool TCPClient::connect(const std::string& host, const std::string& port)
{
    if (isConnected())
        return true; // already connected

    struct addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrInfo);
    if (rc != 0)
    {
        std::cerr << "getaddrinfo failed: " << gai_strerror(rc) << "\n";
        return false;
    }

    sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        freeaddrinfo(addrInfo);
        addrInfo = nullptr;
        return false;
    }

    if (::connect(sock, addrInfo->ai_addr, addrInfo->ai_addrlen) < 0)
    {
        std::cerr << "connect failed: " << std::strerror(errno) << "\n";
        ::close(sock);
        sock = -1;
        freeaddrinfo(addrInfo);
        addrInfo = nullptr;
        return false;
    }

    Utils::log("TCPClient connected to ", host, ":", port);
    return true;
}

void TCPClient::disconnect()
{
    if (sock >= 0)
    {
        ::close(sock);
        sock = -1;
        Utils::log("TCPClient disconnected from ", host, ":", port);
    }
    if (addrInfo != nullptr)
    {
        freeaddrinfo(addrInfo);
        addrInfo = nullptr;
    }
}

bool TCPClient::sendAll(const std::string& data)
{
    if (!isConnected())
    {
        std::cerr << "TCPClient not connected" << "\n";
        return false;
    }

    const char* buf = data.data();
    size_t left = data.size();

    while (left > 0)
    {
        ssize_t sent = ::send(sock, buf, left, 0);
        if (sent < 0)
        {
            std::cerr << "send failed: " << std::strerror(errno) << "\n";
            return false;
        }
        buf += sent;
        left -= sent;
    }

    return true;
}

bool TCPClient::receiveAll(std::string& out, uint32_t timeout)
{
    out.clear();
    if (!isConnected())
    {
        std::cerr << "TCPClient not connected" << "\n";
        return false;
    }

    // timeout value
    struct timeval tv {};
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        std::cerr << "setsockopt failed: " << std::strerror(errno) << "\n";
        return false;
    }

    char buffer[4096]; // char for string append...
    while (true) // read until close or timeout
    {
        ssize_t recvd = ::recv(sock, buffer, sizeof(buffer), 0);
        if (recvd < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                break; // timeout
                
            std::cerr << "recv failed: " << std::strerror(errno) << "\n";
            return false;
        }
        if (recvd == 0)
            break; // remote closed

        out.append(buffer, buffer + recvd);
    }

    return true;
}
