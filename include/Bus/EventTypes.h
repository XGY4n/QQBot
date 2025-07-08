#pragma once
#include <string>

struct WindowLostEvent {
    std::string componentName;
};

struct ShutdownEvent {};
