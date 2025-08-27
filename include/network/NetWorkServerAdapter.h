#include <functional>
#include <string>
#include "httplib.h"
#include <Botlog.h>
#include "tcplib.h"
class NetWorkServerAdapter {
public:

    void addRoute_GET(const std::string& path,
        std::function<void(const httplib::Request&, httplib::Response&)> handler);

    void addRoute_POST(const std::string& path,
        std::function<void(const httplib::Request&, httplib::Response&)> handler);

    void NetWorkServerAdapter::addRoute_TCP(std::function<void(const std::string&, TcpSocket&)> handler);


    void startHttpServer(int port);
    void startTCPServer(int port);
    void stop();

private:
    std::unique_ptr<httplib::Server> server_;
    std::unique_ptr<TcpServer> TcpServer_;

};