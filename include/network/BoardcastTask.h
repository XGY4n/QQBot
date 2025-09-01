#pragma once
#include <string>
#include "NetWorkClientAdapter.h"
#include <QMessage.h>
#include <nlohmann/json.hpp>
#include "message.pb.h"
class BoardcastTask
{ 

public:
	BoardcastTask();
	~BoardcastTask();
	void BoardcastMessage(QMessage msg);

private:
	std::unique_ptr<NetWorkClientAdapter> _adapter;

};