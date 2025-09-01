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
    Boardcast::Message Boardcastmsg;
    msg = msg.toUTF8();
    Boardcastmsg.set_name(msg.name);
    Boardcastmsg.set_qqnumber(msg.QQNumber);
    Boardcastmsg.set_message(msg.message);
    Boardcastmsg.set_datatime(msg.DataTime); 
    Boardcastmsg.set_raw(msg.toString());
           
    std::string serialized;
    if (!Boardcastmsg.SerializeToString(&serialized)) {
        std::cerr << "Failed to serialize message." << std::endl;
        return ;
    }
	_adapter->BoardCastMessage(serialized);
}
