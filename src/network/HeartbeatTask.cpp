#include <network/HeartbeatTask.h>

HeartbeatTask::HeartbeatTask()
{
    _adapter = std::make_unique<HttpClientAdapter>();
}

HeartbeatTask::~HeartbeatTask()
{
    stop();
}

void HeartbeatTask::stop()
{

}

void HeartbeatTask::start()
{


}
void HeartbeatTask::sendHeartbeat(httplib::Client& healthclient)
{
	_adapter->sendHeartbeatTask(healthclient);
}
