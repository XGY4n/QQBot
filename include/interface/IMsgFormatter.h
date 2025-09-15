// IQMsgFormatter.h
#pragma once
#include <string>
#include <sstream>
#include <QMessage.h>

class IMsgFormatter {
public:
    virtual ~IMsgFormatter() = default;
    virtual QMessage parse(std::string& raw) = 0;
};