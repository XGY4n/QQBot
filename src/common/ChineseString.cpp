#include"ChineseString.h"
#include"WinIniWrapper.h"
#include<string>
#include <locale>
#include <codecvt>
#include <tchar.h>
#include <windows.h>
#include <UIAutomation.h>
#include <vector>
typedef enum {
    Bot_Call = 1,
    Bot_NotCall,
    Bot_SelfCall = -2,
}QueueCall;

Chinese::Chinese()
{

}

Chinese::~Chinese()
{

}

wchar_t* Chinese::MBCSToUnicode(wchar_t* buff, const char* str)
{
    wchar_t* wp = buff;
    char* p = (char*)str;
    while (*p)
    {
        if (*p & 0x80)
        {
            *wp = *(wchar_t*)p;
            p++;
        }
        else
        {
            *wp = (wchar_t)*p;
        }
        wp++;
        p++;
    }
    *wp = 0x0000;
    return buff;
}

char* Chinese::unicodeToMBCS(char* buff, const wchar_t* str)
{
    wchar_t* wp = (wchar_t*)str;
    char* p = buff, * tmp;
    while (*wp)
    {
        tmp = (char*)wp;
        if (*wp & 0xFF00)
        {
            *p = *tmp;
            p++;
            tmp++;
            *p = *tmp;
            p++;
        }
        else
        {
            *p = *tmp;
            p++;
        }
        wp++;
    }
    *p = 0x00;
    return buff;
}

std::wstring Chinese::strToWstr(std::string& input)
{
    size_t len = input.size();
    wchar_t* b = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    MBCSToUnicode(b, input.c_str());
    std::wstring r(b);
    free(b);
    return r;
}

char* Chinese::wstrToChar(std::wstring& wstr)
{
    char* re = wstrToChar(wstr.c_str());
    return re;
}

char* Chinese::wstrToChar(const wchar_t* wstr)
{
    int len = wcslen(wstr);
    char* buff = (char*)malloc((len * 2 + 1) * sizeof(char));
    char* re = unicodeToMBCS(buff, wstr);
    free(buff);
    return re;
}

std::string Chinese::wstrToStr(std::wstring& wstr)
{
    size_t len = wstr.size();
    char* b = (char*)malloc((2 * len + 1) * sizeof(char));
    unicodeToMBCS(b, wstr.c_str());
    std::string r(b);
    free(b);
    return r;
}


std::string newone()
{
    CoInitialize(NULL);
    IUIAutomation* pAutomation;
    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
    IUIAutomationElement* pRootElement;
    pAutomation->GetRootElement(&pRootElement);

    IUIAutomationCondition* pNameCondition;
    VARIANT varProp1;
    varProp1.vt = VT_BSTR;
    varProp1.bstrVal = SysAllocString(L"TXGuiFoundation");
    pAutomation->CreatePropertyCondition(UIA_ClassNamePropertyId, varProp1, &pNameCondition);

    IUIAutomationCondition* pClassNameCondition;
    VARIANT varProp2;
    varProp2.vt = VT_BSTR;
    varProp2.bstrVal = SysAllocString(L"消息管理器");
    pAutomation->CreatePropertyCondition(UIA_NamePropertyId, varProp2, &pClassNameCondition);

    IUIAutomationCondition* pAndCondition;
    pAutomation->CreateAndCondition(pNameCondition, pClassNameCondition, &pAndCondition);

    IUIAutomationElement* pWindow;
    pRootElement->FindFirst(TreeScope_Children, pAndCondition, &pWindow);
    // 查找名为 "IEMsgView" 的列表控件
    IUIAutomationCondition* pCondition;
    VARIANT varProp;
    varProp.vt = VT_BSTR;
    varProp.bstrVal = SysAllocString(L"IEMsgView");
    pAutomation->CreatePropertyCondition(UIA_NamePropertyId, varProp, &pCondition);
    IUIAutomationElement* pListElement;
    pRootElement->FindFirst(TreeScope_Descendants, pCondition, &pListElement);
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

    pRootElement->Release();
    pAutomation->Release();
    CoUninitialize();
}


std::string Chinese::cut(std::wstring target, int start, int end) {
    Chinese ch;
    std::string ans;
    std::wstring temp;
    for (int i = start; i <= end; i++) {
        temp += target[i];
    }

    ans = ch.wstrToStr(temp);
    return ans;
}


Chinese::Qmsg Chinese::Qmsgmake2(std::string Rowmessage)
{
    Qmsg result;

    std::wstring input = strToWstr(Rowmessage);
    std::wstring Title, Name, QQnumber, Time, Message;

    int left_set_end_pos = input.find(':');
    std::wstring left_set = input.substr(0, left_set_end_pos);


    int Qnumber_left_symbol = left_set.find('(');
    Qnumber_left_symbol = (Qnumber_left_symbol == std::wstring::npos) ? left_set.find('<') : Qnumber_left_symbol;

    int Qnumber_right_symbol = left_set.find(')');
    Qnumber_right_symbol = (Qnumber_right_symbol == std::wstring::npos) ? left_set.find('>') : Qnumber_right_symbol;



    int Name_left_pos = input.find(49057) + 1;
    int Name_right_pos = Qnumber_left_symbol - 1;

    int Title_left_pos = 0;
    int Title_right_pos = input.find(49057);

    int Time_left_pos = Qnumber_right_symbol + 1;
    int Time_rigth_pos = left_set_end_pos + 5;

    int Msg_left_pos = Time_rigth_pos + 1;
    int Msg_right_pos = input.size();

    std::string name = cut(input, Name_left_pos, Name_right_pos);
    std::string t = cut(input, Title_left_pos, Title_right_pos);
    std::string q = cut(input, Qnumber_left_symbol + 1, Qnumber_right_symbol - 1);
    std::string msg1 = cut(input, Msg_left_pos, Msg_right_pos);
    std::string time = cut(input, Time_left_pos, Time_rigth_pos);

    result.name = name;
    result.QQnumber = q;
    result.DataTime = time;
    result.message = msg1;

    return result;
}

Chinese::Qmsg Chinese::Qmsgmake(std::string Rowmessage)
{
    Qmsg result;

    Chinese ch;
    std::wstring w_str = ch.strToWstr(Rowmessage);
    int neartes_sp = 0;
    int qnum_letf;
    int qnum_right;
    int time_left;
    int time_right;
    int allstart = 0;

    for (int i = w_str.size(); i > 0; i--)
    {
        //std::wcout << w_str[i];
        //std::cout << " " << w_str[i] << " " << i << std::endl;//int(RawMessage[i])
        if (w_str[i] == 32&& w_str[i-1] == 41 && w_str[i + 1] == 32)
        {
            neartes_sp = i;
        }
        if (w_str[i] == 49057)
        {
            allstart = i;
            break;
        }

    }
    if (neartes_sp == 0)
        throw std::out_of_range("WARNING exceeds limit");
    for (int R_qseeker = neartes_sp; R_qseeker != allstart; R_qseeker--)
    {
        if (w_str[R_qseeker] == 41)
        {
            qnum_right = R_qseeker;
            break;
        }

    }
    for (int L_qseeker = qnum_right; L_qseeker > allstart; L_qseeker--)
    {
        //std::cout << w_str[L_qseeker] << std::endl;
        if (w_str[L_qseeker] == 40)
        {
            qnum_letf = L_qseeker;
            break;
        }
    }

    std::wstring sendname4;

    for (int i = allstart + 1; i < qnum_letf; i++)
    {
        sendname4 = sendname4 + w_str[i];
    }
    result.name = ch.wstrToStr(sendname4);
    //std::cout << "size : " << result.name.size() << std::endl;
    if (result.name.size() == 0)
        result.name = "";


    std::wstring sendname3;

    for (int i = qnum_letf + 1; i < qnum_right; i++)
    {
        sendname3 = sendname3 + w_str[i];
    }
    //std::string format 
    result.QQnumber = ch.wstrToStr(sendname3);
    //std::cout << "size : " << ans2.size() << std::endl;
    if (result.QQnumber.size() == 0)
        result.QQnumber = "";
    //time
    for (int L_tseeker = neartes_sp; ; L_tseeker++)
    {
        if (std::isdigit(w_str[L_tseeker]))
        {
            time_left = L_tseeker;
            break;
        }
    }
    for (int R_tseeker = time_left; ; R_tseeker++)
    {
        if (std::isdigit(w_str[R_tseeker]) == false && w_str[R_tseeker] != 58)
        {
            time_right = R_tseeker;
            break;
        }
    }
    std::wstring t;
    for (int i = time_left; i < time_right; i++)
    {
        t = t + w_str[i];
    }
    result.DataTime = ch.wstrToStr(t);
    //std::cout << "size : " << ans3.size() << std::endl;
    if (result.DataTime.size() == 0)
        result.DataTime = "";


    int L_msg = 0;
    int R_msg = w_str.size();

    for (int i = time_right;; i++)
    {
        if (w_str[i] != 13 && w_str[i] != 32)
        {
            L_msg = i;
            break;
        }
           
    }

    std::wstring msg;
    for (int i = L_msg; i < R_msg-2; i++)
    {
        if (w_str[i] != 13)
            msg = msg + w_str[i];
    }
    result.message = ch.wstrToStr(msg);
    //std::cout << "size : " << ans4.size() << std::endl;
    if (result.message.size() == 0)
        result.message = "";

    return result;
}



std::string make_message(std::string RawMessage)//# 35
{
    int ret;
    int start = 0;
    int end = 0;
    int flage = 0;
    Chinese ch;
    std::wstring w_str = ch.strToWstr(RawMessage);
    // for (int i = 0; i < w_str.size(); i++)
    for (int i = w_str.size(); i > 0; i--)
    {
        //cout<<w_str[i]<<endl;//int(RawMessage[i])
        if (w_str[i] == 13 && flage == 0)
        {
            end = i;
            flage = 1;
        }
        else if (w_str[i] == 13 && flage == 1)
        {
            start = i;
            break;
        }
    }
    /*cout << "Start :" << start << endl;
    cout << "end :" << end << endl;*/
    std::wstring sendname;
    for (int i = start + 2; i < end; i++)
    {
        sendname = sendname + w_str[i];
        //cout << w_str[i] << endl;
    }
    std::string ans = ch.wstrToStr(sendname);
    //std::cout << "message: " << ans << std::endl;
    return ans;
}

std::string make_name(std::string RawMessage)
{
    int ret;
    int start = 0;
    int end = 0;
    Chinese ch;
    std::wstring w_str = ch.strToWstr(RawMessage);
    std::wcout << w_str << std::endl;
        for (int i = w_str.size(); i > 0; i--)
        {
            //std::cout << w_str[i] <<" "<<i<< std::endl;//int(RawMessage[i])
            if (start != 0 && end != 0)
            {
                break;
            }
            else
            {
                if (w_str[i] == 49057)
                {
                    start = i;
                }
                else if (w_str[i] == 40 || w_str[i] == 60)
                {
                    end = i;
                }
            }
        }
    std::wstring sendname;

    for (int i = start + 1; i < end; i++)
    {
        sendname = sendname + w_str[i];
    }
    std::string ans = ch.wstrToStr(sendname);
    //std::cout << ans << std::endl;
    return ans;
    //std::cout<<ret<<std::endl;
}

std::string make_number(std::string RawMessage)
{
    int ret;
    int start = 0;
    int end = 0;
    Chinese ch;
    std::wstring w_str = ch.strToWstr(RawMessage);
    // for (int i = 0; i < w_str.size(); i++)
    for (int i = w_str.size(); i > 0; i--)
    {
        //cout<<w_str[i]<<endl;//int(RawMessage[i])
        if (start != 0 && end != 0)
        {
            break;
        }
        else
        {
            if (w_str[i] == 40 || w_str[i] == 60)
            {
                start = i;
            }
            else if (w_str[i] == 41 || w_str[i] == 62)
            {
                end = i;
            }
        }

    }
    std::wstring sendname;
    for (int i = start + 1; i < end; i++)
    {
        sendname = sendname + w_str[i];
        //cout <<w_str[i] << endl;
    }
    std::string ans = ch.wstrToStr(sendname);
    //std::cout << ans << std::endl;
    return ans;
}

int raw_check(std::string RawMessage)
{
    int ret = 0;
     /*if (RawMessage.find("3121913693") != std::string::npos || RawMessage.find("2995244756") != std::string::npos)
         std::cout << "notify " << std::endl;
        //return ret;//1364303391
   
    std::vector<std::string> target_number;
    std::unique_ptr<CInI> Qnumber_reader{ new CInI{"./ini/notify.ini"}};
    Qnumber_reader->GetAllSectionA(target_number);

    for (const std::string& number : target_number) {
        
        if (RawMessage.find(number) != std::string::npos) {
            std::cout <<"notify : " << number << std::endl;
            return ret;  // Return ret if any element in target_number is found in RawMessage
        }
    }*/


    if (RawMessage.find("> ") != std::string::npos)
        return -2;
    ret = RawMessage.find("#");
    //std::cout << "ret :" << RawMessage.size() << std::endl;
    if (ret == 0)
        return 1;
    else
        return -3;
    //std::cout<<"ret :" << ret << std::endl;
    return ret;
}

std::string make_Instructions_message(std::string QueueTop)
{
    //std::cout << QueueTop << std::endl;
    std::string Instructions;
    int start;
    for (int i = QueueTop.size(); i > 0; i--)
    {
        if (QueueTop[i] == 35)
        {
            start = i;
            break;
        }
    }
    for (int i = start+1; i < QueueTop.size(); i++)
    {
        Instructions = Instructions + QueueTop[i];
    }
    return Instructions;
}

std::string make_time(std::string RawMessage)
{
    int start = 0;
    int end = 0;
    std::string time;
    for (int i = RawMessage.size(); i > 0; i--)
    {
        if (RawMessage[i] == 41)
        {
            start = i;
            break;
        }
    }
    start = start + 3;
    end = start + 8;
    for (int i = start; ; i++)//i < end
    {
        if (RawMessage[i] != 58 && (RawMessage[i] < 48 || RawMessage[i]>57))
            break;
        time = time + RawMessage[i];
    }
   // scout << "time :" << time << endl;
    return time;
}

std::string Chinese_SendMessage(std::string CNmessage)
{
    int ret;
    int start = 0;
    int end = CNmessage.size();
    Chinese ch;
    std::wstring w_str = ch.strToWstr(CNmessage);
    std::wstring sendname;
    for (int i = 0; i < 2; i++)
        sendname = sendname + w_str[i];
    std::string ans = ch.wstrToStr(sendname);
    return ans;
}

std::string Random_Number(int range)
{
    int n = 1 + rand() % range;
    return std::to_string(n);
}




std::string make_Instructions_name(std::string QueueTop)
{
    
    std::string Instructions;
    int end;
    for (int i = QueueTop.size(); i > 0; i--)
    {
        if (QueueTop[i] == 35)
        {
            end = i;
            break;
        }
    }
    for (int i = 0; i < end; i++)
        Instructions = Instructions + QueueTop[i];
    return Instructions;
}




std::string make_Instructions_QQnumber(std::string QueueTop)
{
    std::string QQnumber;
    int start;
    int end;
    for (int i = QueueTop.size(); i > 0; i--)
    {
        if (QueueTop[i] == 40)
        {
            start = i;
        }
        if (QueueTop[i] == 41)
        {
            end = i;
        }
    }
    for (int i = start+1; i < end; i++)
        QQnumber = QQnumber + QueueTop[i];
    return QQnumber;
}








std::string Make_Random(std::string Instructions)
{
    std::string rN;
    std::string rC;
    std::string ans;
    if (Instructions.find("d") == std::string::npos)
    {
        ans = "wrong input where is d?";
        return ans;
    }
    int round;
    int RandNumber;
    int start = Instructions.find("r");
    int mid = Instructions.find("d");
    int end = Instructions.size();
    for (int i = start + 2; i < mid; i++)
        rN = rN + Instructions[i];
    for (int i = mid + 1; i < end; i++)
        rC = rC + Instructions[i];
    round = std::stoi(rN);
    RandNumber = std::stoi(rC);
    if (round > 20 || RandNumber >= 999999)
        ans = "x<20 d n<999999";
    else
        for (int i = 0; i < round; i++)
            ans = ans + "骰子" + "["+ std::to_string(i + 1)+"]" + ":" + Random_Number(RandNumber) + " ";
    return ans;
}




std::string GbkToUtf8(const char* src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}


std::string UTF8ToGBK(const char* strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);

    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;

    return strTemp;
}

wchar_t* multi_Byte_To_Wide_Char(std::string pKey)
{
    char pCStrKeyy[10000];
    strcpy(pCStrKeyy, pKey.c_str());
    char* pCStrKey = pCStrKeyy;

    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t* pWCStrKey = new wchar_t[pSize];

    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
    return pWCStrKey;
}



std::string UTF82Char(const char* szU8)
{
    std::string ans;
    int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), NULL, 0);
    wchar_t* wszString = new wchar_t[wcsLen + 1];
    MultiByteToWideChar(CP_UTF8, NULL, szU8, (int)strlen(szU8), wszString, wcsLen);
    wszString[wcsLen] = '\0';
    int len = WideCharToMultiByte(CP_ACP, 0, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);
    char* c = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wszString, (int)wcslen(wszString), c, len, NULL, NULL);
    c[len] = '\0';
    delete[] wszString;
    ans = c;
    return ans;
}


char* wideCharToMultiByte(wchar_t* pWCStrKey)
{

    int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
    char* pCStrKey = new char[pSize + 1];

    WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
    pCStrKey[pSize] = '\0';
    return pCStrKey;
}

std::wstring string2wstring(std::string str)
{
    std::wstring result;
    //获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    TCHAR* buffer = new TCHAR[len + 1];
    //多字节编码转换成宽字节编码  
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';             //添加字符串结尾  
    //删除缓冲区并返回值  
    result.append(buffer);
    delete[] buffer;
    return result;
}
