#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class TcpSocket {
public:
    TcpSocket() : sock(INVALID_SOCKET) { initWSA(); }

    explicit TcpSocket(SOCKET s) : sock(s) { initWSA(); }

    ~TcpSocket() { close(); }

    int sendAll(const std::string& data) {
        return send(sock, data.c_str(), (int)data.size(), 0);
    }

    int recvSome(char* buffer, int bufSize) {
        return recv(sock, buffer, bufSize, 0);
    }

    void close() {
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
    }

    bool isValid() const { return sock != INVALID_SOCKET; }
    SOCKET getRaw() const { return sock; }

private:
    SOCKET sock;

    static void initWSA() {
        static bool initialized = false;
        if (!initialized) {
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("WSAStartup failed");
            }
            initialized = true;
        }
    }
};

class TcpServer {
public:
    TcpServer() : sock(INVALID_SOCKET) {}
    ~TcpServer() { close(); }

    void bindAndListen(const std::string& ip, uint16_t port, int backlog = SOMAXCONN) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("socket() failed");
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            throw std::runtime_error("bind() failed");
        }

        if (listen(sock, backlog) == SOCKET_ERROR) {
            throw std::runtime_error("listen() failed");
        }
    }

    void setHandler(std::function<void(const std::string&, TcpSocket&)> handler) {
        handler_ = std::move(handler);
    }

    void start() 
    {
        _tcpStarted = true;
        LOG_SUCCESS_SELF("TCP server is listening...");
        while (_tcpStarted)
        {
            TcpSocket client = acceptClient();
            char buf[1024];
            while (_tcpStarted)
            {
                int len = client.recvSome(buf, sizeof(buf));
                if (len <= 0) break;
                std::string msg(buf, len);
                if (handler_) 
                {
                    handler_(msg, client);
                }
            }
        }
    }

    void close() 
    {
        _tcpStarted = false;
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
    }

private:
    SOCKET sock;
    std::function<void(const std::string&, TcpSocket&)> handler_;
	bool _tcpStarted = false;
    TcpSocket acceptClient() {
        SOCKET clientSock = accept(sock, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET) {
            throw std::runtime_error("accept() failed");
        }
        return TcpSocket(clientSock);
    }
};


class TcpClient {
public:
    TcpClient() {}

    TcpSocket connectTo(const std::string& ip, uint16_t port) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            throw std::runtime_error("socket() failed");
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

        if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(s);
            throw std::runtime_error("connect() failed");
        }

        return TcpSocket(s);
    }
};
