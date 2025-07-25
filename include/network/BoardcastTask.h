#pragma once
#include <string>
#include "HttpClientAdapter.h"
#include <QMessage.h>
class BoardcastTask
{

public:
	BoardcastTask();
	~BoardcastTask();
	void BoardcastMessage(QMessage msg);

private:
	std::unique_ptr<HttpClientAdapter> _adapter;

};