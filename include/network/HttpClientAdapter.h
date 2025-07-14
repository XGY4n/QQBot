#pragma once

#include <memory>
#include <string>
#include "httplib.h"
#include <Botlog.h>
class HttpClientAdapter {
public:
    HttpClientAdapter();

    void sendHeartbeatTask(int port);

private:
    Botlog* _logger = Botlog::GetInstance();
    std::unique_ptr<httplib::Client> _client;
};