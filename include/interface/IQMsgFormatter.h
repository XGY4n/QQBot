// IQMsgFormatter.h
#pragma once
#include <string>
#include <sstream>
struct QMessage {
    std::string name;
    std::string QQNumber;
    std::string message;
    std::string DataTime;
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << DataTime << "]" << name << "(" << QQNumber << ")" << message;
        return oss.str();
    }
};

class IQMsgFormatter {
public:
    virtual ~IQMsgFormatter() = default;
    virtual QMessage parse(std::string& raw) = 0;
};