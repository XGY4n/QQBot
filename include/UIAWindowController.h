#pragma once
#include <windows.h>
#include <Botlog.h>
#include <interface/IUIAWindowController.h>
#include <Bus/EventBusInstance.h>
class UIAWindowController : public IUIAWindowController {
public:
    explicit UIAWindowController(HWND hwnd);
    void MoveWindowOffScreen();
	~UIAWindowController();
    bool RefreshMsg();
    HWND GetWindow() const;
private:    
    bool SimulateClick(int x, int y);
    HWND _hwnd;
};