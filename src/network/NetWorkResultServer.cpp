#include <network/NetWorkResultServer.h>

NetWorkResultServer::NetWorkResultServer()
{
    _adapter = std::make_unique<NetWorkServerAdapter>();
}
NetWorkResultServer::~NetWorkResultServer()
{
    stop();
}

void NetWorkResultServer::setupRoutes()
{
    // 添加 POST /report 路由
    _adapter->addRoute_POST("/report", [this](const httplib::Request& req, httplib::Response& res) {
        LOG_SUCCESS_SELF("Received POST /report request.");
        LOG_SUCCESS_SELF("POST body: " + req.body);
        if (onReportPostCallback_) {
            onReportPostCallback_(req.body);  // 把数据传回 ServiceManager
        }
        res.set_content("POST /report OK", "text/plain");
    });

    //tpc
    _adapter->addRoute_TCP(
        [this](const std::string& msg, TcpSocket& socket) {
            EventBusInstance::instance().publish(HeartbeatHttpCb{ msg });
    });
}

void NetWorkResultServer::setReportPostCallback(std::function<void(const std::string& body)> cb)
{
    onReportPostCallback_ = std::move(cb);
}

void NetWorkResultServer::start()
{
    _HTTPserverThread = std::thread([this]() {
        _adapter->startHttpServer(11451);
    });
	_TCPserverThread = std::thread([this]() {
		_adapter->startTCPServer(11452);
    });
}

void NetWorkResultServer::stop()
{
    LOG_SUCCESS_SELF("Stopping ResultHttpServer...");
    _adapter->stop();
    if (_HTTPserverThread.joinable()) {
        _HTTPserverThread.join();
    }
	if (_TCPserverThread.joinable()) {
		_TCPserverThread.join();
	}
}