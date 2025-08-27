#pragma once

#include <memory>
#include <string>
#include "httplib.h"
#include <Botlog.h>
#include <QMessage.h>
#include <converSTR.h>
#include <nlohmann/json.hpp>

class NetWorkClientAdapter {
public:
    NetWorkClientAdapter();
    void sendHeartbeatTask(httplib::Client& healthclient);

    void sendHeartbeatTask(SOCKET& healthclient, int port);

    void BoardCastMessage(QMessage msg);

	void Reconnect(SOCKET& healthclient, int port);

private:
    std::unique_ptr<httplib::Client> _boardCastclient;
};