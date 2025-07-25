#pragma once
#include <string>
#include <windows.h>

inline wchar_t* multi_Byte_To_Wide_Char2(std::string pKey)
{
    char pCStrKeyy[10000];
    strcpy(pCStrKeyy, pKey.c_str());
    char* pCStrKey = pCStrKeyy;

    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t* pWCStrKey = new wchar_t[pSize];

    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
    return pWCStrKey;
}