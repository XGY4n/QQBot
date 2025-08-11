#include <functional>
#include <string>
#include "httplib.h"
#include <Botlog.h>
class HttpServerAdapter {
public:

    void addRoute_GET(const std::string& path,
        std::function<void(const httplib::Request&, httplib::Response&)> handler);

    void addRoute_POST(const std::string& path,
        std::function<void(const httplib::Request&, httplib::Response&)> handler);

    void start(int port);
    void stop();

private:
    std::unique_ptr<httplib::Server> server_;
};