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
    void RefreshMsg();
    HWND GetWindow() const;
private:    
    void SimulateClick(int x, int y);

    Botlog* _logger = Botlog::GetInstance();

    HWND _hwnd;
};