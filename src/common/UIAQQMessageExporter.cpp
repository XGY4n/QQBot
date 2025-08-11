#include "UIAQQMessageExporter.h"
#include <sstream>

UIAQQMessageExporter::UIAQQMessageExporter()
{
    start();
}

UIAQQMessageExporter::~UIAQQMessageExporter()
{
    stop();
}

void UIAQQMessageExporter::stop()
{
    std::lock_guard<std::mutex> lock(_windowMutex);
	_alive = false;
    _window.clear();
    cv.notify_all();
    CoUninitialize();
}

void UIAQQMessageExporter::start()
{
    _alive = true;

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to initialize COM");
    }

    hr = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IUIAutomation), (void**)&pAutomation);
    if (FAILED(hr) || pAutomation == nullptr) {
        CoUninitialize();
        throw std::runtime_error("Failed to create UIAutomation instance");
    }

    pAutomation->GetRootElement(&pRootElement);

    VARIANT varProp1;
    varProp1.vt = VT_BSTR;
    varProp1.bstrVal = SysAllocString(L"TXGuiFoundation");
    pAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp1, &pNameCondition);

    VARIANT varProp2;
    varProp2.vt = VT_BSTR;
    varProp2.bstrVal = SysAllocString(L"消息管理器");
    pAutomation->CreatePropertyCondition(UIA_NamePropertyId, varProp2, &pClassNameCondition);

    pAutomation->CreateAndCondition(pNameCondition, pClassNameCondition, &pAndCondition);

    pRootElement->FindFirst(TreeScope_Children, pAndCondition, &pWindow);

    VARIANT varProp;
    varProp.vt = VT_BSTR;
    varProp.bstrVal = SysAllocString(L"IEMsgView");
    pAutomation->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pCondition);

    pWindow->FindFirst(TreeScope_Descendants, pCondition, &pListElement);
}

std::string UIAQQMessageExporter::ConvertBSTRToString(BSTR bstr)
{
    if (bstr == nullptr)
    {
        return "";
    }

    int bstrLength = SysStringLen(bstr);
    if (bstrLength <= 0) 
    {
        return "";
    }

    int stringLength = WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, NULL, 0, NULL, NULL);
    if (stringLength <= 0) 
    {
        return "";
    }

    char* buffer = new (std::nothrow) char[stringLength + 1];
    if (buffer == nullptr)
    {
        return "";
    }

    int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, nullptr, 0, nullptr, nullptr);
    std::string result(sizeNeeded, '\0');
    WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, result.data(), sizeNeeded, nullptr, nullptr);
    return result;
}

HRESULT UIAQQMessageExporter::ProcessUIAElement(IUIAutomationElement* pItemElement, std::vector<std::string>& ItemsList)
{
    BSTR bstrName = nullptr;
    HRESULT hr = pItemElement->get_CurrentName(&bstrName);

    if (FAILED(hr) || bstrName == nullptr)
        return S_FALSE;

    std::string str = ConvertBSTRToString(bstrName);
    SysFreeString(bstrName); // 释放 BSTR

    if (str.empty())
        return S_FALSE;

    if (lastStr == str)
    {
        ItemsList.clear();
        return S_OK;
    }

    ItemsList.push_back(str);
    return hr;
}

std::vector<std::string> UIAQQMessageExporter::GetQQMessages()
{
    if (!_firstTime && GetQQMessagesLastOne() == lastStr) {
        return {};
    }
    ItemsList.clear();
    if (!pListElement) return {};

    IUIAutomationTreeWalker* pControlWalker = nullptr;
    HRESULT hr = pAutomation->get_ControlViewWalker(&pControlWalker);
    if (FAILED(hr) || !pControlWalker) return {};

    IUIAutomationElement* pItemElement = nullptr;
    hr = pControlWalker->GetFirstChildElement(pListElement, &pItemElement);
    while (SUCCEEDED(hr) && pItemElement)
    {
        ProcessUIAElement(pItemElement, ItemsList);

        IUIAutomationElement* pNextItemElement = nullptr;
        hr = pControlWalker->GetNextSiblingElement(pItemElement, &pNextItemElement);

        pItemElement->Release();
        pItemElement = pNextItemElement;
    }

    pControlWalker->Release();

    if (!ItemsList.empty()) 
    {
        lastStr = ItemsList[ItemsList.size() - 1];
    }

    {
        std::lock_guard<std::mutex> lock(_windowMutex);

        for (const auto& str : ItemsList) 
        {     
            _window.push_back(str);
        }		
    }

    cv.notify_one();

    return ItemsList;
}

std::string UIAQQMessageExporter::GetQQMessage()
{
    if (!_firstTime) {
        lastStr = PopMessage();
        return lastStr;
    }

    std::string tempstr = GetQQMessagesLastOne();
    lastStr = tempstr;
    _firstTime = false;
    return lastStr;
}

std::string UIAQQMessageExporter::PopMessage()
{
    std::unique_lock<std::mutex> lock(_windowMutex);
    cv.wait(lock, [this] { return !_window.empty() || !_alive ; });
    if (!_window.empty())
    {
        std::string msg = std::move(_window.front());    
        _window.pop_front();
        return msg;
    }
    return std::string{};
}


std::string UIAQQMessageExporter::GetQQMessagesLastOne()
{
    std::string error = "error";
    if (pListElement == nullptr || pAutomation == nullptr)
        return error;

    CComPtr<IUIAutomationTreeWalker> pControlWalker;
    HRESULT hr = pAutomation->get_ControlViewWalker(&pControlWalker);
    if (FAILED(hr) || !pControlWalker)
        return error;

    CComPtr<IUIAutomationElement> pItemElement;
    hr = pControlWalker->GetLastChildElement(pListElement, &pItemElement);
    if (FAILED(hr) || !pItemElement)
        return error;

    CComBSTR bstrName;
    hr = pItemElement->get_CurrentName(&bstrName);
    if (FAILED(hr) || !bstrName)
        return error;

    int length = SysStringLen(bstrName);
    if (length == 0)
        return "";

    int stringLength = WideCharToMultiByte(CP_ACP, 0, bstrName, length, NULL, 0, NULL, NULL);
    if (stringLength == 0)
        return "";

    std::string result(stringLength, 0);
    WideCharToMultiByte(CP_ACP, 0, bstrName, length, &result[0], stringLength, NULL, NULL);

    return result;
}