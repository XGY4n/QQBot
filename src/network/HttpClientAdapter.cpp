#include <network/HttpClientAdapter.h>

void HttpClientAdapter::sendHeartbeatTask(int port)
{
    _healthclient = std::make_unique<httplib::Client>("127.0.0.1", port);

    auto response = _healthclient->Get("/health");
    if (response->status != 200) 
    {
        _logger->LOG_WARNING_SELF("GET 127.0.0.1:"+ std::to_string(port) + "/health");
    } 
}

void HttpClientAdapter::BoardCastMessage(QMessage msg)
{
    if (!_boardCastclient) {
        _logger->LOG_ERROR_SELF("HTTP client nullptr");
        return;
    }

    httplib::Headers headers = {
        { "Content-Type", "application/json" }
    };
    nlohmann::json j;
    j["message"] = multi_Byte_To_Wide_Char2(msg.toString());
    std::string body = j.dump();

    auto res = _boardCastclient->Post("/broadcast", headers, body, "application/json");

    if (res && res->status == 200) 
    {
        _logger->LOG_SUCCESS_SELF("broadcast success" + msg.toString());
    }
    else 
    {
        _logger->LOG_ERROR_SELF("broadcast error" );
    }
}


HttpClientAdapter::HttpClientAdapter()
{
    _boardCastclient = std::make_unique<httplib::Client>("127.0.0.1", 19198);
}