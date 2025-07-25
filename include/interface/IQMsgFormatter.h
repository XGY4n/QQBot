// IQMsgFormatter.h
#pragma once
#include <string>
#include <sstream>
#include <QMessage.h>

class IQMsgFormatter {
public:
    virtual ~IQMsgFormatter() = default;
    virtual QMessage parse(std::string& raw) = 0;
};