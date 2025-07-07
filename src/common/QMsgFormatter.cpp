#include <QMsgFormatter.h>

std::wstring QMsgFormatter::strToWstr(std::string& input)
{
    size_t len = input.size();
    wchar_t* b = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
    MBCSToUnicode(b, input.c_str());
    std::wstring r(b);
    free(b);
    return r;
}

wchar_t* QMsgFormatter::MBCSToUnicode(wchar_t* buff, const char* str)
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

std::string QMsgFormatter::cut(std::wstring target, int start, int end) {
    std::string ans;
    std::wstring temp;
    for (int i = start; i <= end; i++) {
        temp += target[i];
    }

    ans = wstrToStr(temp);
    return ans;
}

std::string QMsgFormatter::wstrToStr(std::wstring& wstr)
{
    size_t len = wstr.size();
    char* b = (char*)malloc((2 * len + 1) * sizeof(char));
    unicodeToMBCS(b, wstr.c_str());
    std::string r(b);
    free(b);
    return r;
}
char* QMsgFormatter::unicodeToMBCS(char* buff, const wchar_t* str)
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
QMessage QMsgFormatter::parse(std::string& raw) 
{
    QMessage result;
    std::wstring input = strToWstr(raw);

    int left_set_end_pos = input.find(':');
    std::wstring left_set = input.substr(0, left_set_end_pos);

    int Qnumber_left_symbol = left_set.find('(');
    if (Qnumber_left_symbol == std::wstring::npos)
        Qnumber_left_symbol = left_set.find('<');

    int Qnumber_right_symbol = left_set.find(')');
    if (Qnumber_right_symbol == std::wstring::npos)
        Qnumber_right_symbol = left_set.find('>');

    int Name_left_pos = input.find(49057) + 1; // Unicode 特殊字符
    int Name_right_pos = Qnumber_left_symbol - 1;

    int Title_left_pos = 0;
    int Title_right_pos = input.find(49057);

    int Time_left_pos = Qnumber_right_symbol + 1;
    int Time_rigth_pos = left_set_end_pos + 5;

    int Msg_left_pos = Time_rigth_pos + 1;
    int Msg_right_pos = input.size();

    // 使用 cut 提取字段
    result.name = cut(input, Name_left_pos, Name_right_pos);
    result.QQNumber = cut(input, Qnumber_left_symbol + 1, Qnumber_right_symbol - 1);
    result.message = cut(input, Msg_left_pos, Msg_right_pos);
    result.DataTime = cut(input, Time_left_pos, Time_rigth_pos);

    return result;
}


