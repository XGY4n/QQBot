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
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _subscribers.find(typeid(EventType));
        if (it != _subscribers.end()) {
            auto sptr = std::make_shared<EventType>(event);
            for (auto& handler : it->second) {
                handler(sptr);
            }
        }
    }


private:
    std::unordered_map<std::type_index, std::vector<std::function<void(std::shared_ptr<void>)>>> _subscribers;
    std::mutex _mutex;
};
