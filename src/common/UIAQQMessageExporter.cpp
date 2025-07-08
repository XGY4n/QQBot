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
    varProp2.bstrVal = SysAllocString(L"��Ϣ������");
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


// ������������ BSTR ת��Ϊ std::string
std::string UIAQQMessageExporter::ConvertBSTRToString(BSTR bstr)
{
    if (bstr == nullptr)
    {
        return "";
    }

    int bstrLength = SysStringLen(bstr);
    if (bstrLength <= 0) // ����Ϊ0����
    {
        return "";
    }

    // �������軺������С
    int stringLength = WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, NULL, 0, NULL, NULL);
    if (stringLength <= 0) // ת��ʧ�ܻ򳤶�Ϊ0
    {
        return "";
    }

    char* buffer = new (std::nothrow) char[stringLength + 1]; // ʹ�� new (std::nothrow) ��ֹ�ڴ����ʧ��
    if (buffer == nullptr)
    {
        // �ڴ����ʧ�ܴ���
        // ���Լ�¼��־���׳��쳣
        return "";
    }

    WideCharToMultiByte(CP_ACP, 0, bstr, bstrLength, buffer, stringLength, NULL, NULL);
    buffer[stringLength] = '\0';
    std::string str(buffer);
    delete[] buffer;
    return str;
}

// ���������������� UI Automation Ԫ�ز��ռ�����
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

    // ���� lastStr �� _window ���߼�
    if (!debug_list_items.empty()) // ȷ�� debug_list_items ��Ϊ��
    {
        lastStr = debug_list_items[debug_list_items.size() - 1];
    }

    { //lock 
        std::lock_guard<std::mutex> lock(_windowMutex);
        for (const auto& str : debug_list_items) // ʹ�� const auto& ���ⲻ��Ҫ�Ŀ���
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
        // ��ȡ�б�ؼ�����Ԫ��
        IUIAutomationTreeWalker* pControlWalker;
        pAutomation->get_ControlViewWalker(&pControlWalker);
        IUIAutomationElement* pItemElement;
        pControlWalker->GetLastChildElement(pListElement, &pItemElement);
        if (pItemElement != NULL)
        {
            // ��ȡ���������һ���б������Ϣ
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
