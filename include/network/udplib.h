#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

class UdpClient
{
public:
	UdpClient(const std::string& ip = "", unsigned short port = 0)
	{
		
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			throw std::runtime_error("WSAStartup failed");
		}

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			throw std::runtime_error("socket creation failed");
		}
		if (!ip.empty() && port != 0) {
			defaultIp = ip;
			defaultPort = port;
		}
	}

	~UdpClient()
	{
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
		}
		WSACleanup();
	}

	void enableBroadcast()
	{
		int enable = 1;
		setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
			(char*)&enable, sizeof(enable));
	}

	void send(const std::string& ip, unsigned short port, const std::string& msg)
	{
		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

		sendto(sock, msg.c_str(), (int)msg.size(), 0,
			(sockaddr*)&addr, sizeof(addr));
	}

	void send(const std::string& msg)
	{
		if (defaultIp.empty()) {
			throw std::runtime_error("No default IP/port set");
		}
		send(defaultIp, defaultPort, msg);
	}
private:
	SOCKET sock{ INVALID_SOCKET };
	std::string defaultIp;
	unsigned short defaultPort{ 0 };
};

class UdpServer
{
public:
	UdpServer(unsigned short port)
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			throw std::runtime_error("WSAStartup failed");
		}

		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			throw std::runtime_error("socket creation failed");
		}

		sockaddr_in addr{};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);

		if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
			closesocket(sock);
			WSACleanup();
			throw std::runtime_error("bind failed");
		}
	}

	~UdpServer()
	{
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
		}
		WSACleanup();
	}

	std::string recv(std::string& senderIp, unsigned short& senderPort)
	{
		char buffer[1024];
		sockaddr_in sender{};
		int senderLen = sizeof(sender);

		int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
			(sockaddr*)&sender, &senderLen);
		if (len == SOCKET_ERROR) {
			throw std::runtime_error("recvfrom failed");
		}

		buffer[len] = '\0';
		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &sender.sin_addr, ipStr, sizeof(ipStr));
		senderIp = ipStr;
		senderPort = ntohs(sender.sin_port);

		return std::string(buffer, len);
	}

private:
	SOCKET sock{ INVALID_SOCKET };
};
