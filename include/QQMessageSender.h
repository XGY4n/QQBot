#pragma once

#include <string>
#include <windows.h>
#include <mutex>
#include <filesystem>
#include "ChineseString.h"
#include "Botlog.h"
#include "MD5.h"
#include <WinIniWrapper.h>
namespace fs = std::filesystem;

class QQMessageSender {
public:
    QQMessageSender(HWND Group);

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

    // �ļ�ϵͳ����
    std::string GenerateTempFile(const std::string& filePath);
    bool EnsureFolderExists(const std::string& folderPath);
    bool FileExists(const std::string& filePath);
    std::string GetFullPath(const std::string& path);

private:
    Botlog* logger = Botlog::GetInstance();
    HWND _group;
    std::string m_groupName;
    std::mutex m_mutex;
};

