#include "network/HttpServerAdapter.h"

void HttpServerAdapter::addRoute_GET(const std::string& path,
    std::function<void(const httplib::Request&, httplib::Response&)> handler)
{
    if (!server_) {
        server_ = std::make_unique<httplib::Server>();
    }
    LOG_SUCCESS_SELF("Registering GET route: " + path);
    server_->Get(path.c_str(), std::move(handler));
}

void HttpServerAdapter::addRoute_POST(const std::string& path,
    std::function<void(const httplib::Request&, httplib::Response&)> handler)
{
    if (!server_) {
        server_ = std::make_unique<httplib::Server>();
    }
    LOG_SUCCESS_SELF("Registering POST route: " + path);
    server_->Post(path.c_str(), std::move(handler));
}

void HttpServerAdapter::start(int port)
{
    if (!server_) {
        server_ = std::make_unique<httplib::Server>();
    }

    LOG_SUCCESS_SELF("Starting HTTP server on port " + std::to_string(port));
    if (!server_->listen("0.0.0.0", port)) {
        LOG_ERROR_SELF("Failed to start HTTP server on port " + std::to_string(port));
    }
    else {
        LOG_SUCCESS_SELF("HTTP server is listening on port " + std::to_string(port));
    }
}

void HttpServerAdapter::stop()
{
    if (server_) {
        LOG_SUCCESS_SELF("Stopping HTTP server...");
        server_->stop();
    }
}
