#pragma once

#include <string>
#include <windows.h>
#include <nlohmann/json.hpp>
#include <TaskBuilder.h>

// QQ ��Ϣ���ͽӿ�
class IMessageFeedback {
public:
    virtual ~IMessageFeedback() = default;

    // �ı���Ϣ����
    virtual void SendTextToGroup(const std::string& message) = 0;
    virtual void SendTextToUser(const std::string& message, const std::string& userName) = 0;

    // �ļ�����
    virtual void SendFileToGroup(const std::string& filePath) = 0;
    virtual void SendFileToUser(const std::string& filePath, const std::string& userName) = 0;

    // ͼƬ����
    virtual void SendImageToGroup(const std::string& imagePath) = 0;

    // ͨ�÷���
    virtual void sendMessage(const std::string& content) = 0;
    virtual void sendMessageAsJson(const std::string& jsonInfo, QMessage callInfo) = 0;

    // ����Ⱥ������
    virtual void SetGroupName(const std::string& groupName) = 0;

    // @ ָ�� QQ
    virtual void sendMessageAt(const std::string QQnumber) = 0;
};
