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
void HeartbeatTask::sendHeartbeat(std::string uuid, int heartbeatPort)
{
	_adapter->sendHeartbeatTask(heartbeatPort);
}
