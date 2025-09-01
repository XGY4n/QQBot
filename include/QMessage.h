// QMessage.h
#pragma once
#include <string>
#include <sstream>
#include <converSTR.h>
struct QMessage {
    std::string name;
    std::string QQNumber;
    std::string message;
    std::string DataTime;

    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << DataTime << "] " << name << " (" << QQNumber << ") " << message;
        return oss.str();
    }

    QMessage toUTF8() const {
        QMessage utf8Msg;
        utf8Msg.name = GbkToUTF8(name.c_str());
        utf8Msg.QQNumber = GbkToUTF8(QQNumber.c_str());
        utf8Msg.message = GbkToUTF8(message.c_str());
        utf8Msg.DataTime = GbkToUTF8(DataTime.c_str());
        return utf8Msg;
    }
};
