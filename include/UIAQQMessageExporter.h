#pragma once

#include <string>
#include <vector>
//#include <windows.h>
#include <UIAutomation.h>
#include <iostream>
#include <interface/IQQMessageExporter.h>
#include <deque>
#include <mutex>
#include <atlbase.h>
#include <atlcomcli.h>
using ATL::CComPtr;

class UIAQQMessageExporter : public IQQMessageExporter
{
public:
    UIAQQMessageExporter();
    ~UIAQQMessageExporter();
public:
    std::string GetQQMessage();
    std::vector<std::string> GetQQMessageList();
    std::vector<std::string> GetQQMessages();
public:
    IUIAutomationElement* pRootElement;
    IUIAutomation* pAutomation;
    IUIAutomationCondition* pNameCondition;
    IUIAutomationCondition* pAndCondition;
    IUIAutomationCondition* pClassNameCondition;
    IUIAutomationCondition* pCondition;
    IUIAutomationElement* pListElement;
    IUIAutomationElement* pWindow;
private :
    HRESULT ProcessUIAElement(IUIAutomationElement* pItemElement, std::vector<std::string>& debug_list_items);
    std::string ConvertBSTRToString(BSTR bstr);

    std::mutex _windowMutex;
    std::string lastStr;
    std::deque<std::string> _window;
};

