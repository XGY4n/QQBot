#pragma once
#include <string>
//#include <windows.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <comutil.h>  
#include <tchar.h> 
#include <unknwn.h>
#include <queue>
#include <thread>
#include <stdio.h>
#include <optional>
#include <type_traits>
#include <deque>
#ifdef UNICODE
#define FindValue FindValueW
#else
#define FindValue FindValueA
#endif

#ifdef UNICODE
#define CInIW WinInIWrapper
#else
#define CInIA CInI
#endif

#ifdef UNICODE
#define InIexist InIexistW
#else
#define InIexist InIexistA
#endif

#ifdef UNICODE
#define GetAllSection GetAllSectionW
#else
#define GetAllSection GetAllSectionA
#endif


#ifdef UNICODE
#define GetAllSectionKey GetAllSectionKeyW
#else
#define GetAllSectionKey GetAllSectionKeyA
#endif

#ifdef UNICODE
#define CountSection CountSectionW
#else
#define CountSection CountSectionA
#endif

#ifdef UNICODE
#define CountSectionKey CountSectionKeyW
#else
#define CountSectionKey CountSectionKeyA
#endif



#ifdef UNICODE
#define ReadAll ReadAllW
#else
#define ReadAll ReadAllA
#endif

#ifdef UNICODE
#define WriteValue WriteValueW
#else
#define WriteValue WriteValueA
#endif


#define _MAPTYPE_ 1
#define _PAIRTYPE_ 2


class WinInIWrapper
{

public:
    template<typename T>
    T  FindValueW(const std::wstring& section, const std::wstring& name)
    {

        std::wstring value = InIfindW(section, name);
        if constexpr (std::is_same<T, std::wstring>::value)
            return value;
        else if constexpr (std::is_same<T, std::string>::value)
            return wchUTF82StringUFT8(value.c_str());
        else
        {
            std::wstringstream ss(value);
            T result;
            ss >> result;
            return result;
        }
    }

    template<typename T>
    T FindValueA(const std::string& section, const std::string& name)
    {
#if defined(NDEBUG) && defined(UNICODE)
#pragma message("Warning: UNICODE use")
#endif
        std::string value = InIfindA(section, name);

        if (value.empty())
        {
            return T{};//std::string();
        }

        if constexpr (std::is_same<T, std::string>::value)
            return value;
        else if constexpr (std::is_same<T, std::wstring>::value)
            return std::wstring(value.begin(), value.end());
        else if constexpr (std::is_same<T, bool>::value)
        {
            std::string lowerVal = value;
            std::transform(lowerVal.begin(), lowerVal.end(), lowerVal.begin(), ::tolower);
            return (lowerVal == "true" || lowerVal == "1");
        }
        else
        {

            std::stringstream ss(value);
            T result;
            ss >> result;
            if (ss.fail() || ss.bad() || !ss.eof())
            {
                return T{};/*std::string();*/
            }
            return result;
        }
    }
    template<typename RA>
    RA ReadAllT() {
        RA result;
        // Process the data and store the result in result
        // ...
        return result;
    }
    /*template<typename RA>
    RA ReadAllA(RA& data)
    {
        if constexpr (std::is_same_v<RA, INI_MAP<std::wstring>>)
            return 1;
        else //if constexpr (std::is_same<T, std::wstring>::value)
            return 2;
    }
    template<typename RA>
    RA ReadAllW()
    {
        if constexpr (std::is_same_v<RA, INI_MAP<std::wstring>>)
            return 1;
        else //if constexpr (std::is_same<T, std::wstring>::value)
            return 2;
    }*/

public:
    typedef enum {
        INI_READ_PATHERROR = -0x03,
        INI_WRITE_ERROR = -0x02,
        INI_READ_SECKEY_ERR = 0x00,
        INI_READ_SECKEY_SUCC = 0x01,

        INI_READ_NOTEXIST = 0x00,
        INI_READ_FIND = 0x01,
        INI_READ_EXIST = 0x01,

        INI_WRITE_ERR = 0,
        INI_WRITE_SUCC = 1
    }InIexstat;

public:
    //#ifdef UNICODE#else#endif
    template<typename T_INI>
    struct InIMapping {
        T_INI section;
        std::map<T_INI, T_INI> parameters;
    };
    template<typename T_INI>
    struct INI_PAIR {
        T_INI section;
        std::vector<std::pair<T_INI, T_INI>> parameters;
    };
    using M_InIData = InIMapping<std::wstring>;
    using P_InIData = INI_PAIR<std::wstring>;

    /* template<typename T_INI>
     struct INI_MAP {
         std::string section;
         std::map<std::string, T_INI> parameters;
     };
     template<typename T_INI>
     struct INI_PAIR {
         std::string section;
         std::pair<std::string, T_INI> parameters;
     };
     using M_InIData = INI_MAP<std::string>;
     using P_InIData = INI_PAIR<std::string>;*/



public:

    WinInIWrapper(const std::wstring& inifile);

    WinInIWrapper(const std::string& inifile);
    ~WinInIWrapper();

public:

    std::string InIfindA(std::string section, std::string name);
    std::wstring InIfindW(std::wstring section, std::wstring name);

    InIexstat InIexistA(const std::string& section, const std::string& name = "", const std::string& value = "");
    InIexstat InIexistW(const std::wstring& section, const std::wstring& name = L"", const std::wstring& value = L"");

    InIexstat InIWrite(std::string msg);
    InIexstat InIWriteW(std::wstring msg);

    InIexstat GetAllSectionA(std::vector<std::string>& section_list);
    InIexstat GetAllSectionW(std::vector<std::wstring>& section_list);

    InIexstat GetAllSectionKeyA(std::vector<std::string>& section_list, std::vector<std::string>& sectionkey_list);
    InIexstat GetAllSectionKeyW(std::vector<std::wstring>& section_list, std::vector<std::wstring>& sectionkey_list);

    InIexstat GetAllSectionKeyA(const std::string& sectionName, std::vector<std::string>& sectionkey_list);
    InIexstat GetAllSectionKeyW(const std::wstring& sectionName, std::vector<std::wstring>& sectionkey_list);

    int CountSectionA();
    int CountSectionW();

    int CountSectionKeyA(const std::string& section);
    int CountSectionKeyW(const std::wstring& section);


    std::vector<WinInIWrapper::INI_PAIR<std::wstring>> ReadAllW(int);
    std::vector<InIMapping<std::wstring>> ReadAllW();

    std::vector<WinInIWrapper::InIMapping<std::string>> ReadAllA();
    std::vector<WinInIWrapper::INI_PAIR< std::string>> ReadAllA(const int);

    InIexstat WriteValueA(const std::string& section, const std::string& key, const std::string& value);
    InIexstat WriteValueW(const std::wstring& section, const std::wstring& key, const std::wstring& value);

    bool IsValid();

private:
    std::string target_ini_path;
    std::wstring wtarget_ini_path;

    LPCSTR lpFileNameA;
    LPCWSTR lpFileNameW;

private:
    char* wchUTF82StringUFT8(const  wchar_t* pWCStrKey);
    //std::string wstrToStr(std::wstring& wstr);
    wchar_t* MBCSToUnicode(wchar_t* buff, const char* str);
    std::wstring strToWstr(std::string input);

};

