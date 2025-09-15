#pragma once

#include <string>
#include <vector>
class IMessageExporter {
public:
    virtual ~IMessageExporter() = default;
    virtual std::string GetQQMessage() = 0;
    virtual std::vector<std::string> GetQQMessages() = 0;
    virtual void stop() = 0;
    virtual void start() = 0;
};
