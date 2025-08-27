#include <network/httplib.h>
#include <network/NetWorkServerAdapter.h>
#include <Bus/EventBusInstance.h>


class NetWorkResultServer {
public:
    NetWorkResultServer();
    ~NetWorkResultServer();
    void setupRoutes();
    void start();
    void stop();
    void setReportPostCallback(std::function<void(const std::string& body)> cb);
private :    


private:
    std::thread _HTTPserverThread;
    std::thread _TCPserverThread;

    bool _running = true;
    std::function<void(const std::string& body)> onReportPostCallback_;
    std::unique_ptr<NetWorkServerAdapter> _adapter;
};