#pragma once

#include <string>
#include <vector>
#include <UIAutomation.h>
#include <iostream>
#include <interface/IQQMessageExporter.h>
#include <deque>
#include <mutex>
#include <atlbase.h>
#include <atlcomcli.h>
#include <atlcom.h> 

using ATL::CComPtr;

class UIAQQMessageExporter : public IQQMessageExporter
{
public:
    UIAQQMessageExporter();
    ~UIAQQMessageExporter();
public:
    std::string GetQQMessage();
    std::vector<std::string> GetQQMessages();
    void stop();
    void start();
public:
    CComPtr<IUIAutomationElement> pRootElement;
    CComPtr<IUIAutomation> pAutomation;
    CComPtr<IUIAutomationCondition> pNameCondition;
    CComPtr<IUIAutomationCondition> pAndCondition;
    CComPtr<IUIAutomationCondition> pClassNameCondition;
    CComPtr<IUIAutomationCondition> pCondition;
    CComPtr<IUIAutomationElement> pListElement;
    CComPtr<IUIAutomationElement> pWindow;
private :
    HRESULT ProcessUIAElement(IUIAutomationElement* pItemElement, std::vector<std::string>& ItemsList);
    std::string ConvertBSTRToString(BSTR bstr);
    std::string GetQQMessagesLastOne();
    std::string PopMessage();

    std::mutex _windowMutex;
    std::string lastStr;
    std::deque<std::string> _window;
    bool _firstTime = true;
    std::condition_variable cv;
    std::vector<std::string> ItemsList;

    std::atomic<bool> _alive{ false };

};

