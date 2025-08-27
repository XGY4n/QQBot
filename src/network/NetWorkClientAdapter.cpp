#include <network/NetWorkClientAdapter.h>


void NetWorkClientAdapter::sendHeartbeatTask(httplib::Client& healthclient)
{
    try 
    {
        auto response = healthclient.Get("/health");
        if (!response) {
            LOG_WARNING_SELF("GET /health failed: response is null. Port: " +
                std::to_string(healthclient.port()
            ));
        }
        else if (response->status != 200) 
        {
            LOG_WARNING_SELF("GET 127.0.0.1:"+ 
                std::to_string(healthclient.port()) + "/health");
        } 
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_SELF(std::string("Exception: ") + e.what());
    }
}

void NetWorkClientAdapter::sendHeartbeatTask(SOCKET& healthclient, int port)
{
    SOCKET sock = healthclient;
    const std::string& msg = "beat";

    int sent = send(sock, msg.data(), static_cast<int>(msg.size()), 0);
    if (sent == SOCKET_ERROR)
    {
        Reconnect(healthclient, port);
    }
}
void NetWorkClientAdapter::Reconnect(SOCKET& healthclient, int port)
{
    int err = WSAGetLastError();
    closesocket(healthclient);
    healthclient = INVALID_SOCKET;

    SOCKET newSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    healthclient = newSock;
    if (newSock == INVALID_SOCKET)
    {
        LOG_ERROR_SELF("Failed to create new socket, WSA error: " + std::to_string(WSAGetLastError()));
    }
    else
    {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(newSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            LOG_ERROR_SELF("Reconnect failed, WSA error: " + std::to_string(WSAGetLastError()));
            closesocket(newSock);
        }
        else
        {
            //LOG_SUCCESS_SELF("connect success");
            healthclient = newSock;
        }
    }
}


void NetWorkClientAdapter::BoardCastMessage(QMessage msg)
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    int broadcastEnable = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(19198);
    addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // π„≤•µÿ÷∑
    nlohmann::json j;
    j["message"] = multi_Byte_To_Wide_Char2(msg.toString());
    std::string body = j.dump();
    sendto(sock, body.c_str(), (int)body.size(), 0,
        (sockaddr*)&addr, sizeof(addr));
     
    closesocket(sock);
    WSACleanup();
} 


NetWorkClientAdapter::NetWorkClientAdapter()
{
    _boardCastclient = std::make_unique<httplib::Client>("127.0.0.1", 19198);
}