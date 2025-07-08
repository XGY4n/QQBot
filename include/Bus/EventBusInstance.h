#pragma once
#include "EventBus.h"

class EventBusInstance {
public:
    static EventBus& instance() {
        static EventBus bus;
        return bus;
    }
};
