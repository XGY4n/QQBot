#include <UIAWindowController.h>

UIAWindowController::UIAWindowController(HWND hwnd) : _hwnd(hwnd) {
	if (_hwnd == nullptr) {
		_logger->LOG_ERROR_SELF("Invalid window handle provided to UIAWindowController");
	}
}

UIAWindowController:: ~UIAWindowController() {
	if (_hwnd != nullptr) {
		_logger->LOG_SUCCESS_SELF("UIAWindowController destroyed successfully");
	}
}

bool UIAWindowController::RefreshMsg()
{
	return SimulateClick(646, 113);
}


bool UIAWindowController::SimulateClick(int x, int y)
{
	if (!IsWindow(_hwnd))
	{
		return false;
	}
	LPARAM lParam = MAKELPARAM(x, y);
	PostMessage(_hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
	PostMessage(_hwnd, WM_LBUTTONUP, 0, lParam);
	return true;
}

HWND UIAWindowController::GetWindow() const
{
	return this->_hwnd;
}

void UIAWindowController::MoveWindowOffScreen()
{
	// 获取屏幕分辨率
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 获取窗口大小
	RECT rect;
	GetWindowRect(_hwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// 将窗口移动到屏幕外
	MoveWindow(_hwnd, screenWidth, screenHeight, windowWidth, windowHeight, TRUE);
}