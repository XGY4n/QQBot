#include <string>
#include "NetWorkClientAdapter.h"
class HeartbeatTask 
{
public:
    HeartbeatTask();
    ~HeartbeatTask();
    void start();
    void stop();    
    void sendHeartbeat(httplib::Client& healthclient);
    void sendHeartbeatTCP(SOCKET& healthclient, int port);
private:
    std::unique_ptr<NetWorkClientAdapter> _adapter;
    std::string target_url;
};