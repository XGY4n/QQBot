#pragma once
#include <string>
#include <QMessage.h>
struct WindowLostEvent 
{
    std::string componentName;
};

struct HeartbeatHttpCb
{
    std::string body;
};

struct ShutdownEvent {};


struct PythonCallbackInfo
{
    std::string HttpBody;
    QMessage callInfo;
};

struct FetchMessageEvent
{
	QMessage msg;
};

struct TaskConfigReloadEvent
{
};