#include <network/ResultHttpServer.h>

ResultHttpServer::ResultHttpServer()
{
    _adapter = std::make_unique<HttpServerAdapter>();
}
ResultHttpServer::~ResultHttpServer()
{
    stop();
}

void ResultHttpServer::setupRoutes()
{
    // 添加 POST /report 路由
    _adapter->addRoute_POST("/report", [this](const httplib::Request& req, httplib::Response& res) {
        _logger->LOG_SUCCESS_SELF("Received POST /report request.");
        _logger->LOG_SUCCESS_SELF("POST body: " + req.body);
        if (onReportPostCallback_) {
            onReportPostCallback_(req.body);  // 把数据传回 ServiceManager
        }
        res.set_content("POST /report OK", "text/plain");
    });
}

void ResultHttpServer::setReportPostCallback(std::function<void(const std::string& body)> cb)
{
    onReportPostCallback_ = std::move(cb);
}

void ResultHttpServer::start()
{
    _serverThread = std::thread([this]() {
        _adapter->start(11451); 
        });

    _serverThread.detach();  
}

void ResultHttpServer::stop()
{
    _logger->LOG_SUCCESS_SELF("Stopping ResultHttpServer...");
    _adapter->stop();
}