#pragma once

#include <memory>
#include <string>
#include "httplib.h"
#include <Botlog.h>
#include <QMessage.h>
#include <converSTR.h>
#include "udplib.h"
class NetWorkClientAdapter 
{
public:
    NetWorkClientAdapter();

    void sendHeartbeatTask(httplib::Client& healthclient);

    void sendHeartbeatTask(SOCKET& healthclient, int port);

    void BoardCastMessage(std::string serialized);

	void Reconnect(SOCKET& healthclient, int port);

private:
    std::unique_ptr<UdpClient> _boardCastclientUDP;
};