#include <string>
#include "HttpClientAdapter.h"
class HeartbeatTask 
{
public:
    HeartbeatTask();
    ~HeartbeatTask();
    void start();
    void stop();    
    void sendHeartbeat(httplib::Client& healthclient);
private:
    std::unique_ptr<HttpClientAdapter> _adapter;
    std::string target_url;
};