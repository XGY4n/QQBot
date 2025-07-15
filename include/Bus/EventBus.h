#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <typeindex>
#include <memory>
#include "EventTypes.h"  
class EventBus 
{
public:
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> callback) {
        static_assert(!std::is_same<EventType, void>::value, "Cannot subscribe to void type");

        std::lock_guard<std::mutex> lock(_mutex);
        auto& handlers = _subscribers[typeid(EventType)];
        handlers.push_back([callback](std::shared_ptr<void> event) {
            callback(*std::static_pointer_cast<EventType>(event));
        });
    }

    template<typename EventType>
    void publish(const EventType& event) {
        std::shared_ptr<EventType> sptr = std::make_shared<EventType>(event);

        std::vector<std::function<void(std::shared_ptr<void>)>> syncHandlers;
        std::vector<std::function<void(std::shared_ptr<void>)>> asyncHandlers;

        {
            std::lock_guard<std::mutex> lock(_mutex);
            auto it = _subscribers.find(typeid(EventType));
            if (it != _subscribers.end()) {
                syncHandlers = it->second;
            }
            auto itAsync = _asyncSubscribers.find(typeid(EventType));
            if (itAsync != _asyncSubscribers.end()) {
                asyncHandlers = itAsync->second;
            }
        }

        for (auto& handler : syncHandlers) {
            handler(sptr);
        }
        for (auto& handler : asyncHandlers) {
            handler(sptr);
        }
    }

    template<typename EventType>
    void asyncSubscribe(std::function<void(const EventType&)> callback) {
        static_assert(!std::is_same<EventType, void>::value, "Cannot subscribe to void type");

        std::lock_guard<std::mutex> lock(_mutex);
        auto& handlers = _asyncSubscribers[typeid(EventType)];
        handlers.push_back([callback, this](std::shared_ptr<void> event) {
            auto actualEvent = std::static_pointer_cast<EventType>(event);
            _executor([callback, actualEvent]() {
                callback(*actualEvent);  
                });            
            });
    }

    void setExecutor(std::function<void(std::function<void()>)> executor) {
        std::lock_guard<std::mutex> lock(_mutex);
        _executor = std::move(executor);
    }

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(std::shared_ptr<void>)>>> _subscribers;
    std::unordered_map<std::type_index, std::vector<std::function<void(std::shared_ptr<void>)>>> _asyncSubscribers;
    std::function<void(std::function<void()>)> _executor;

    std::mutex _mutex;
};
