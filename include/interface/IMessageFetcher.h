// IMessageFetcher.h
#pragma once
#include <functional>

class IMessageFetcher {
public:
    virtual ~IMessageFetcher() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
public:
    using MessageCallback = std::function<void(const QMessage&)>;
    virtual void SetMessageCallback(MessageCallback cb) = 0;
};
