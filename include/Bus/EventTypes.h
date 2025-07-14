#pragma once
#include <string>
#include <interface/IQMsgFormatter.h>
struct WindowLostEvent 
{
    std::string componentName;
};

struct HeartbeatHttpCb
{
    std::string body;
};

struct ShutdownEvent {};


struct HttpCallbackInfo
{
    std::string HttpBody;
    QMessage callInfo;
};