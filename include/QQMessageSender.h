#pragma once

#include <string>
#include <windows.h>
#include <mutex>
#include <filesystem>
#include "ChineseString.h"
#include "Botlog.h"
#include "MD5.h"
#include <WinIniWrapper.h>
#include <nlohmann/json.hpp>
#include <TaskBuilder.h>
#include <set>

namespace fs = std::filesystem;

class QQMessageSender {
public:
    QQMessageSender(HWND Group);

    void setGroubHandle(HWND Group)
    {
        _group = Group;

    }
    // �ı���Ϣ����
    void SendTextToGroup(const std::string& message);
    void SendTextToUser(const std::string& message, const std::string& userName);

    // �ļ�����
    void SendFileToGroup(const std::string& filePath);
    void SendFileToUser(const std::string& filePath, const std::string& userName);

    // ͼƬ����
    void SendImageToGroup(const std::string& imagePath);

    // ͨ�÷��ͷ������Զ��ж����ͣ�
    void sendMessage(const std::string& content);
    void sendMessageAsJson(const std::string& JsonInfo, QMessage callinfo);
    // ����Ⱥ������
    void SetGroupName(const std::string& groupName);
    void sendMessageAt(std::string QQnumber);

    //void sendMessageAt(const std::string& content, std::string QQnumber);
private:
    // ���ڲ���
    HWND FindTargetWindow(const std::string& targetName);
    void SendStringToWindow( const std::string& message);
    void SendFileToWindow(const std::string& filePath);
    void SendImageToWindow(const std::string& imagePath);
    //void sendMessageAt(std::string QQnumber);
    // ���������
    bool CopyFileToClipboard(const char* szFileName);
    bool AddRichEditFormatToClipboard(const std::string& targetPath);
    bool IsImageFileExtension(const std::string& extension);

    // �ļ�ϵͳ����
    std::string GenerateTempFile(const std::string& filePath);
    bool EnsureFolderExists(const std::string& folderPath);
    bool FileExists(const std::string& filePath);
    std::string GetFullPath(const std::string& path);

    void handleAtMessage(bool isAtback, const std::string& callInfoJson, const std::string& QQNumber, const std::string& name);

    bool ParseJsonInfo(const std::string& jsonStr,
        std::string& uuid,
        std::string& status,
        std::string& timestamp,
        std::string& return_type,
        std::string& values);

    bool IsNumeric(const std::string& str);

    void SendFileOrFallback(const std::string& path);

    void SendImageOrFallback(const std::string& path);
private:
    const std::string _kSupportedImageTypes = ".jpg, .jpeg, .png, .bmp, .gif";
    HWND _group;
    std::string m_groupName;
    std::mutex m_mutex;
    std::unique_ptr<WinInIWrapper> _botConfig;
    bool isAtback = false;
};

