#include "QQMessageSender.h"
#include <comutil.h>
#include <shlobj.h>
#include <io.h>
#include <fstream>
#include <vector>
#include <algorithm>


QQMessageSender::QQMessageSender(HWND Group) 
{
    _group = Group;
}

void QQMessageSender::sendMessageAt(std::string QQnumber)
{
    SetForegroundWindow(_group);
    std::string s = "@" + QQnumber;
    setlocale(LC_ALL, "chs");
    TCHAR text[5000];
    _stprintf_s(text, 5000, _T("%S"), s.c_str());
    for (int i = 0; i < lstrlen(text); i++)
        SendMessage(_group, WM_IME_CHAR, text[i], 0);
    Sleep(50);
    SendMessage(_group, WM_KEYDOWN, VK_RETURN, 0);
    Sleep(50);
}


// 文本消息发送实现
void QQMessageSender::SendTextToGroup(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!_group) return;

    std::string formatted = "> " + message;
    SendStringToWindow(formatted);
}

void QQMessageSender::SendTextToUser(const std::string& message, const std::string& userName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!_group) return;

    std::string formatted = "> " + message;
    SendStringToWindow(formatted);
}

// 文件发送实现
void QQMessageSender::SendFileToGroup(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!_group) return;

    SendFileToWindow(filePath);
}

void QQMessageSender::SendFileToUser(const std::string& filePath, const std::string& userName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!_group) return;

    SendFileToWindow(filePath);
}

// 图片发送实现
void QQMessageSender::SendImageToGroup(const std::string& imagePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!_group) return;

    SendImageToWindow(imagePath);
}

// 通用发送方法实现
void QQMessageSender::sendMessage(const std::string& content) {
    if (fs::exists(GetFullPath(content))) {
        std::string ext = fs::path(content).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        if (ext == ".jpg" || ext == ".gif" || ext == ".png" || ext == ".bmp") {
            SendImageToGroup(content);
        }
        else {
            SendFileToGroup(content);
        }
    }
    else {
        SendTextToGroup(content);
    }
}

// 设置群聊名称
void QQMessageSender::SetGroupName(const std::string& groupName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_groupName = groupName;
}

// 私有方法实现
HWND QQMessageSender::FindTargetWindow(const std::string& targetName) {
    HWND hwnd = FindWindow(_T("TXGuiFoundation"), multi_Byte_To_Wide_Char(targetName));
    if (!hwnd && logger) {
        logger->LOG_ERROR_SELF("Window not found: " + targetName);
    }
    return hwnd;
}

void QQMessageSender::SendStringToWindow(const std::string& message) {
    setlocale(LC_ALL, "chs");
    TCHAR text[5000];
    _stprintf_s(text, 5000, _T("%S"), message.c_str());

    for (int i = 0; i < lstrlen(text); i++) {
        SendMessage(_group, WM_IME_CHAR, text[i], 0);
    }

    Sleep(50);
    SendMessage(_group, WM_KEYDOWN, VK_RETURN, 0);
    Sleep(50);
}

void QQMessageSender::SendFileToWindow( const std::string& filePath) {
    if (!FileExists(filePath)) {
        if (logger) logger->LOG_ERROR_SELF("File not found: " + filePath);
        return;
    }

    if (CopyFileToClipboard(filePath.c_str())) {
        SendMessageA(_group, WM_PASTE, 0, 0);
        Sleep(100);
        SendMessageA(_group, WM_KEYDOWN, VK_RETURN, 0);
        Sleep(100);
        if (logger) logger->LOG_SUCCESS_SELF("File sent: " + filePath);
    }
    else if (logger) {
        logger->LOG_ERROR_SELF("Failed to copy file to clipboard: " + filePath);
    }
}

void QQMessageSender::SendImageToWindow(const std::string& imagePath) {
    std::string tempFile = GenerateTempFile(imagePath);
    if (!tempFile.empty() && AddRichEditFormatToClipboard(tempFile)) {
        SendMessage(_group, WM_PASTE, 0, 0);
        Sleep(100);
        SendMessage(_group, WM_KEYDOWN, VK_RETURN, 0);
        Sleep(100);
    }
}

bool QQMessageSender::CopyFileToClipboard(const char* szFileName) {
    UINT uDropEffect = RegisterClipboardFormat(L"Preferred DropEffect");
    if (!uDropEffect) return false;

    HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
    if (!hGblEffect) return false;

    LPDWORD lpdDropEffect = static_cast<LPDWORD>(GlobalLock(hGblEffect));
    *lpdDropEffect = DROPEFFECT_COPY;
    GlobalUnlock(hGblEffect);

    DROPFILES stDrop = { sizeof(DROPFILES), {0, 0}, FALSE, FALSE };
    size_t pathLen = strlen(szFileName) + 1;
    HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,
        sizeof(DROPFILES) + pathLen);
    if (!hGblFiles) {
        GlobalFree(hGblEffect);
        return false;
    }

    LPSTR lpData = static_cast<LPSTR>(GlobalLock(hGblFiles));
    memcpy(lpData, &stDrop, sizeof(DROPFILES));
    strcpy_s(lpData + sizeof(DROPFILES), pathLen, szFileName);
    GlobalUnlock(hGblFiles);

    if (OpenClipboard(nullptr)) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, hGblFiles);
        SetClipboardData(uDropEffect, hGblEffect);
        CloseClipboard();
        return true;
    }

    GlobalFree(hGblFiles);
    GlobalFree(hGblEffect);
    return false;
}

bool QQMessageSender::AddRichEditFormatToClipboard(const std::string& targetPath) {
    if (!FileExists(targetPath)) return false;

    if (FileExists(targetPath)) {// && file.substr(file.length() - 4) == ".gif")
        std::string qqRichEditData = "<QQRichEditFormat><Info version=\"1001\"></Info><EditElement type=\"7\" filepath=\"" + targetPath + "\" shortcut=\"\"></EditElement></QQRichEditFormat>";
        std::vector<char> data(qqRichEditData.begin(), qqRichEditData.end());
        logger->LOG_SUCCESS_SELF(qqRichEditData);
        //std::cout << data << std::endl;
        // Put data into clipboard
        if (OpenClipboard(0)) {
            EmptyClipboard();

            HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, data.size());
            if (hglbCopy != 0) {
                LPVOID lptstrCopy = GlobalLock(hglbCopy);
                memcpy(lptstrCopy, data.data(), data.size());
                GlobalUnlock(hglbCopy);
                UINT formatQQUnicodeRichEdit = RegisterClipboardFormat(LPCWSTR(L"QQ_Unicode_RichEdit_Format"));
                SetClipboardData(formatQQUnicodeRichEdit, hglbCopy);

                //UINT formatQQRichEdit = RegisterClipboardFormat(LPCWSTR(L"QQ_RichEdit_Format"));
                //SetClipboardData(formatQQRichEdit, hglbCopy);1

                CloseClipboard();
                return true;
            }
        }
    }
    return false;
}

std::string QQMessageSender::GenerateTempFile(const std::string& filePath) {
    if (!FileExists(filePath)) return "";

    std::string tempDir = GetFullPath(".\\tempimg\\");
    if (!EnsureFolderExists(tempDir)) return "";

    md5::MD5 md;
    std::string hash = md.digestFile(const_cast<char*>(filePath.c_str()));
    std::string ext = fs::path(filePath).extension().string();
    std::string tempFilePath = tempDir + hash + ext;

    if (FileExists(tempFilePath)) return tempFilePath;

    if (CopyFileA(filePath.c_str(), tempFilePath.c_str(), FALSE)) {
        return tempFilePath;
    }

    if (logger) logger->LOG_ERROR_SELF("Failed to create temp file: " + tempFilePath);
    return "";
}

bool QQMessageSender::EnsureFolderExists(const std::string& folderPath) {
    DWORD attrib = GetFileAttributesA(folderPath.c_str());
    if (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }

    if (CreateDirectoryA(folderPath.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }

    if (logger) logger->LOG_ERROR_SELF("Failed to create directory: " + folderPath);
    return false;
}

bool QQMessageSender::FileExists(const std::string& filePath) {
    DWORD attrib = GetFileAttributesA(filePath.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES &&
        !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::string QQMessageSender::GetFullPath(const std::string& path) {
    char buffer[MAX_PATH];
    if (GetFullPathNameA(path.c_str(), MAX_PATH, buffer, NULL)) {
        return buffer;
    }
    return path;
}