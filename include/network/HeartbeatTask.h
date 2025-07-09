#include <string>
#include "HttpClientAdapter.h"
class HeartbeatTask 
{
public:
    HeartbeatTask();
    ~HeartbeatTask();
    void start();
    void stop();    
    void sendHeartbeat(std::string uuid, int heartbeatPort);
private:
    std::unique_ptr<HttpClientAdapter> _adapter;
    std::string target_url;
};