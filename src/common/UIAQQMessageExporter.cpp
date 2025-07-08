#include "UIAQQMessageExporter.h"
#include <sstream>

UIAQQMessageExporter::UIAQQMessageExporter()
{
    CoInitialize(NULL);

    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);

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

UIAQQMessageExporter::~UIAQQMessageExporter()
{
    pRootElement->Release();
    pAutomation->Release();
    CoUninitialize();
}


// 辅助函数：将 BSTR 转换为 std::string
std::string UIAQQMessageExporter::ConvertBSTRToString(BSTR bstr)
{
    if (bstr == nullptr)
    {
        return "";
    }

    int bstrLength = SysStringLen(bstr);
    if (bstrLength <= 0) // 长度为0或负数
    {
        return "";
    }

    // 计算所需缓冲区大小
    int stringLength = WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, NULL, 0, NULL, NULL);
    if (stringLength <= 0) // 转换失败或长度为0
    {
        return "";
    }

    char* buffer = new (std::nothrow) char[stringLength + 1]; // 使用 new (std::nothrow) 防止内存分配失败
    if (buffer == nullptr)
    {
        // 内存分配失败处理
        // 可以记录日志或抛出异常
        return "";
    }

    WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, buffer, stringLength, NULL, NULL);
    buffer[stringLength] = '\0';
    std::string str(buffer);
    delete[] buffer;
    return str;
}

// 辅助函数：处理单个 UI Automation 元素并收集数据
HRESULT UIAQQMessageExporter::ProcessUIAElement(IUIAutomationElement* pItemElement, std::vector<std::string>& debug_list_items)
{
    BSTR bstrName = nullptr;
    HRESULT hr = pItemElement->get_CurrentName(&bstrName);

    if (SUCCEEDED(hr) && bstrName != nullptr)
    {
        std::string str = ConvertBSTRToString(bstrName);
        SysFreeString(bstrName); // release BSTR 

        if (!str.empty())
        {
            if (lastStr == str)
            {
                debug_list_items.clear(); 
				return S_OK; 
            }
            debug_list_items.push_back(str);
        }
    }
    return hr;
}

std::vector<std::string> UIAQQMessageExporter::GetQQMessages()
{
    std::vector<std::string> debug_list_items;

    if (pListElement != NULL)
    {
        IUIAutomationTreeWalker* pControlWalker = nullptr;
        HRESULT hr = pAutomation->get_ControlViewWalker(&pControlWalker);

        if (SUCCEEDED(hr) && pControlWalker != NULL)
        {
            IUIAutomationElement* pItemElement = nullptr;
            hr = pControlWalker->GetFirstChildElement(pListElement, &pItemElement);

            while (SUCCEEDED(hr) && pItemElement != NULL)
            {
                ProcessUIAElement(pItemElement, debug_list_items);

                IUIAutomationElement* pNextItemElement = nullptr;
                hr = pControlWalker->GetNextSiblingElement(pItemElement, &pNextItemElement);

                pItemElement->Release();
                pItemElement = pNextItemElement;
            }
            pControlWalker->Release();
        }
    }

    // 处理 lastStr 和 _window 的逻辑
    if (!debug_list_items.empty()) // 确保 debug_list_items 不为空
    {
        lastStr = debug_list_items[debug_list_items.size() - 1];
    }

    { //lock 
        std::lock_guard<std::mutex> lock(_windowMutex);
        for (const auto& str : debug_list_items) // 使用 const auto& 避免不必要的拷贝
        {
            _window.push_back(str);
        }
    }
    return debug_list_items;
}

std::string UIAQQMessageExporter::GetQQMessage()
{
    if (HandleFirstMessage())
        return lastStr;

    return PopMessage();
}

bool UIAQQMessageExporter::HandleFirstMessage()
{
    std::lock_guard<std::mutex> lock(_windowMutex);

    if (_firstTime)
    {
        std::string tempstr = GetQQMessagesLastOne(); 
        _firstTime = false;
        lastStr = tempstr;
        return true;
    }
    return false;
}

std::string UIAQQMessageExporter::PopMessage()
{
    std::lock_guard<std::mutex> lock(_windowMutex);

    if (_window.empty()) {
        return "N/A";
    }

    std::string msg = _window.front();
    _window.pop_front();
    return msg;
}


std::string UIAQQMessageExporter::GetQQMessagesLastOne()
{
    //std::lock_guard<std::mutex> lock(_windowMutex);
    std::string error = "error";
    if (pListElement != NULL)
    {
        // 获取列表控件的子元素
        IUIAutomationTreeWalker* pControlWalker;
        pAutomation->get_ControlViewWalker(&pControlWalker);
        IUIAutomationElement* pItemElement;
        pControlWalker->GetLastChildElement(pListElement, &pItemElement);
        if (pItemElement != NULL)
        {
            // 获取并处理最后一个列表项的信息
            BSTR bstrName;
            pItemElement->get_CurrentName(&bstrName);
            //wprintf(L"%s\n", bstrName);

            std::wstring ws(bstrName, ::SysStringLen(bstrName));
            ::SysFreeString(bstrName);
            int bstrLength = SysStringLen(bstrName);
            int stringLength = WideCharToMultiByte(CP_ACP, 0, bstrName, bstrLength, NULL, 0, NULL, NULL);
            char* buffer = new char[stringLength + 1];

            WideCharToMultiByte(CP_ACP, 0, bstrName, bstrLength, buffer, stringLength, NULL, NULL);

            buffer[stringLength] = '\0';
            std::string str(buffer);
            pItemElement->Release();
            delete[] buffer;
            SysFreeString(bstrName);
            return str;
        }
    }
    return error;
}
