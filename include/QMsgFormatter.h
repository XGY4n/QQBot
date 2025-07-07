#pragma once
#include <string>
#include <locale>
#include <codecvt>
#include <tchar.h>
#include <windows.h>
#include <UIAutomation.h>
#include <vector>
#include "interface/IQMsgFormatter.h"

class QMsgFormatter : public IQMsgFormatter {


public:
    QMessage parse(std::string& raw) ;

private:

    std::wstring strToWstr(std::string& input);
	wchar_t* MBCSToUnicode(wchar_t* buff, const char* str);
	char* unicodeToMBCS(char* buff, const wchar_t* str);
    std::string wstrToStr(std::wstring& wstr);
    std::string QMsgFormatter::cut(std::wstring target, int start, int end);
        
};
