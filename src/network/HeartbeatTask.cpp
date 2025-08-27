#include <network/HeartbeatTask.h>

HeartbeatTask::HeartbeatTask()
{
    _adapter = std::make_unique<NetWorkClientAdapter>();
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
void HeartbeatTask::sendHeartbeatTCP(SOCKET& healthclient, int port)
{
	_adapter->sendHeartbeatTask(healthclient, port);
}

void HeartbeatTask::sendHeartbeat(httplib::Client& healthclient)
{
	_adapter->sendHeartbeatTask(healthclient);
}
