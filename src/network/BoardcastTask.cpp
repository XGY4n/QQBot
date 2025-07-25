#include <network/BoardcastTask.h>

BoardcastTask::BoardcastTask()
{
	_adapter = std::make_unique<HttpClientAdapter>();

}
BoardcastTask::~BoardcastTask()
{

}

void BoardcastTask::BoardcastMessage(QMessage msg)
{
	_adapter->BoardCastMessage(msg);
}
