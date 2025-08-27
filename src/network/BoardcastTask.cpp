#include <network/BoardcastTask.h>

BoardcastTask::BoardcastTask()
{
	_adapter = std::make_unique<NetWorkClientAdapter>();

}
BoardcastTask::~BoardcastTask()
{

}

void BoardcastTask::BoardcastMessage(QMessage msg)
{
	_adapter->BoardCastMessage(msg);
}
