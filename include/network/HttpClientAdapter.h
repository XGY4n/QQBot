#pragma once

#include <memory>
#include <string>
#include "httplib.h"
#include <Botlog.h>
#include <QMessage.h>
#include <converSTR.h>
#include <nlohmann/json.hpp>

class HttpClientAdapter {
public:
    HttpClientAdapter();

    void sendHeartbeatTask(int port);

    void HttpClientAdapter::BoardCastMessage(QMessage msg);

private:
    Botlog* _logger = Botlog::GetInstance();
    std::unique_ptr<httplib::Client> _healthclient;
    std::unique_ptr<httplib::Client> _boardCastclient;

};