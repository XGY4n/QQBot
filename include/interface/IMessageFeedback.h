#pragma once

#include <string>
#include <windows.h>
#include <nlohmann/json.hpp>
#include <TaskBuilder.h>

// QQ 消息发送接口
class IMessageFeedback {
public:
    virtual ~IMessageFeedback() = default;

    // 文本消息发送
    virtual void SendTextToGroup(const std::string& message) = 0;
    virtual void SendTextToUser(const std::string& message, const std::string& userName) = 0;

    // 文件发送
    virtual void SendFileToGroup(const std::string& filePath) = 0;
    virtual void SendFileToUser(const std::string& filePath, const std::string& userName) = 0;

    // 图片发送
    virtual void SendImageToGroup(const std::string& imagePath) = 0;

    // 通用发送
    virtual void sendMessage(const std::string& content) = 0;
    virtual void sendMessageAsJson(const std::string& jsonInfo, QMessage callInfo) = 0;

    // 设置群聊名称
    virtual void SetGroupName(const std::string& groupName) = 0;

    // @ 指定 QQ
    virtual void sendMessageAt(const std::string QQnumber) = 0;
};
