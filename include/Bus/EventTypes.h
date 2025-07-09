#pragma once
#include <string>

struct WindowLostEvent 
{
    std::string componentName;
};

struct HeartbeatHttpCb
{
    std::string body;
};

struct ShutdownEvent {};

