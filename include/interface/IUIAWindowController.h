// IUIAWindowController.h
#pragma once

#include <windows.h>

class IUIAWindowController {
public:
    virtual ~IUIAWindowController() = default;
    virtual void MoveWindowOffScreen() = 0;
    virtual HWND GetWindow() const = 0;
    virtual bool RefreshMsg() = 0;
    // 根据实际需要添加其他纯虚函数
};
