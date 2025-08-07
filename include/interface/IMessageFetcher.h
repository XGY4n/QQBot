// IMessageFetcher.h
#pragma once
#include <functional>

class IMessageFetcher {
public:
    virtual ~IMessageFetcher() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};
