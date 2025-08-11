#include <network/httplib.h>
#include <network/HttpServerAdapter.h>
#include <Bus/EventBusInstance.h>


class ResultHttpServer {
public:
    ResultHttpServer();
    ~ResultHttpServer();
    void setupRoutes();
    void start();
    void stop();
    void setReportPostCallback(std::function<void(const std::string& body)> cb);
private :    


private:
    std::thread _serverThread;
    std::function<void(const std::string& body)> onReportPostCallback_;
    std::unique_ptr<HttpServerAdapter> _adapter;
};