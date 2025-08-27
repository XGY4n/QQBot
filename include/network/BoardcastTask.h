#pragma once
#include <string>
#include "NetWorkClientAdapter.h"
#include <QMessage.h>
class BoardcastTask
{

public:
	BoardcastTask();
	~BoardcastTask();
	void BoardcastMessage(QMessage msg);

private:
	std::unique_ptr<NetWorkClientAdapter> _adapter;

};