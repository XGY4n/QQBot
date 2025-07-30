#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <thread>
#include <memory>
#include <sstream>
#include <type_traits>
#include <codecvt>
#include <iterator>
#include <mutex>

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_RESET_W   L"\033[0m"
#define COLOR_RED_W     L"\033[31m"
#define COLOR_GREEN_W   L"\033[32m"
#define COLOR_YELLOW_W  L"\033[33m"
#define COLOR_CYAN_W    L"\033[36m"
#define COLOR_MAGENTA_W L"\033[35m"

#ifdef _CLASS_LOG_
#define LOG(level, owner, message) \
        Botlog::Record(level, owner, \
        std::string("[") + __FUNCTION__ + "] " + message)
#else
#define LOG(level, owner, message) Botlog::Record(level, owner, message)
#endif
#define LOG_WARNING_SELF(message) LOG(Botlog::Level::LEVEL_WARNIGN, Botlog::OWNER_SELF, message)
#define LOG_WARNING_USERCALL(message) LOG(Botlog::Level::LEVEL_WARNIGN, Botlog::OWNER_USERCALL, message)
#define LOG_WARNING_UNDEF(message) LOG(Botlog::Level::LEVEL_WARNIGN, Botlog::OWNER_UNDEF, message)
#define LOG_ERROR_SELF(message) LOG(Botlog::Level::LEVEL_ERROR, Botlog::OWNER_SELF, message)
#define LOG_ERROR_USERCALL(message) LOG(Botlog::Level::LEVEL_ERROR, Botlog::OWNER_USERCALL, message)
#define LOG_ERROR_UNDEF(message) LOG(Botlog::Level::LEVEL_ERROR, Botlog::OWNER_UNDEF, message)
#define LOG_SUCCESS_SELF(message) LOG(Botlog::Level::LEVEL_SUCCESS, Botlog::OWNER_SELF, message)
#define LOG_SUCCESS_USERCALL(message) LOG(Botlog::Level::LEVEL_SUCCESS, Botlog::OWNER_USERCALL, message)
#define LOG_SUCCESS_UNDEF(message) LOG(Botlog::Level::LEVEL_SUCCESS, Botlog::OWNER_UNDEF, message)

#define _WARNING  COLOR_YELLOW "[WARNING]" COLOR_RESET
#define _ERROR    COLOR_RED "[ERROR]" COLOR_RESET
#define _SUCCESS  COLOR_GREEN "[SUCCESS]" COLOR_RESET
#define _SELF     COLOR_CYAN "[C++]" COLOR_RESET
#define _CALL     COLOR_GREEN "[CALL]" COLOR_RESET
#define _NOTCALL  COLOR_MAGENTA "[NOT CALL]" COLOR_RESET
#define _INFO     COLOR_CYAN "[INFO]" COLOR_RESET
#define _MIDCUT   " "
#define _SYSTEM   COLOR_MAGENTA "[SYSTEM]" COLOR_RESET
#define _MSG      "[MESSAGE]"

#define IS_DIGIT_STR(str) \
    ([](const string& s) -> bool { \
        for (char c : s) { \
            if (!isdigit(c)) return false; \
        } \
        return true; \
    })(str)

#define _WARNING_W  COLOR_YELLOW_W L"[WARNING]" COLOR_RESET_W
#define _ERROR_W    COLOR_RED_W L"[ERROR]" COLOR_RESET_W
#define _SUCCESS_W  COLOR_GREEN_W L"[SUCCESS]" COLOR_RESET_W
#define _SELF_W     COLOR_CYAN_W L"[C++]" COLOR_RESET_W
#define _CALL_W     COLOR_GREEN_W L"[CALL]" COLOR_RESET_W
#define _NOTCALL_W  COLOR_MAGENTA_W L"[NOT CALL]" COLOR_RESET_W
#define _INFO_W     COLOR_CYAN_W L"[INFO]" COLOR_RESET_W
#define _MIDCUT_W   L" "
#define _SYSTEM_W   COLOR_MAGENTA_W L"[SYSTEM]" COLOR_RESET_W
#define _MSG_W      L"[MESSAGE]"

#define _STR 1
#define _WSTR 0

static const char* kDays[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static const char* kDaysAbbrev[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

static const char* kMonths[12] = { "January", "February", "March", "April", "May", "June", "July", "August",
                                    "September", "October", "November", "December" };

static const char* lev_section[3] = { _WARNING ,_ERROR, _SUCCESS };
static const char* own_section[3] = { _SELF ,_CALL,  _NOTCALL };
static const char* sys_section[2] = { _MSG ,_SYSTEM };


static const wchar_t* lev_sectionW[3] = { _WARNING_W ,_ERROR_W, _SUCCESS_W };
static const wchar_t* own_sectionW[3] = { _SELF_W ,_CALL_W,  _NOTCALL_W };

static const char* kMonthsAbbrev[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char* kDefaultDateTimeFormat = "[%d-%02d-%02d %02d:%02d:%02d]";
static const char* kmsDateTimeFormat = "[%d-%02d-%02d %02d:%02d:%02d.%03llu]";
static const wchar_t* kDefaultDateTimeFormatW = L"[%d-%02d-%02d %02d:%02d:%02d]";
static const char* kDefaultDateTimeFormatInFilename = "[%d-%02d-%02d]";
static const int kYearBase = 1900;
static const char* kAm = "AM";
static const char* kPm = "PM";
enum class Level : int {
    /// @brief Generic level that represents all the levels. Useful when setting global configuration for all levels
    Global = 1,
    /// @brief Information that can be useful to back-trace certain events - mostly useful than debug logs.
    Trace = 2,
    /// @brief Informational events most useful for developers to debug application
    Debug = 4,
    /// @brief Severe error information that will presumably abort application
    Fatal = 8,
    /// @brief Information representing errors in application but application will keep running
    Error = 16,
    /// @brief Useful when application has potentially harmful situations
    Warning = 32,
    /// @brief Information that can be highly useful and vary with verbose logging level.
    Verbose = 64,
    /// @brief Mainly useful to represent current progress of application
    Info = 128,
    /// @brief Represents unknown level
    Unknown = 1010
};
inline std::string GetSimpleClassName(const std::string& full_name) {
    size_t pos = full_name.rfind("::");
    if (pos != std::string::npos)
        return full_name.substr(pos + 2);
    return full_name;
}
class Botlog
{
public:
    Botlog(const std::string& logpath = "./log/");
    ~Botlog();
    static  Botlog* GetInstance() {
        static Botlog instance;
        return &instance;
    }
private:
    bool FileExists(const std::string& path);
    bool CreateFileIfNotExists(const std::string& path);
    bool FolderExists(const std::string& path);
    bool CreateFolderIfNotExists(const std::string& path);
public:
    typedef enum {
        LOG_WRITE_ERR,
        LOG_WRITE_SUCCESS,
    }Logstat;

    typedef enum {
        LEVEL_WARNIGN = 0x00,
        LEVEL_ERROR = 0x01,
        LEVEL_SUCCESS = 0x02,
    }Level;

    typedef enum {
        OWNER_SELF = 0x00,
        OWNER_USERCALL = 0x01,
        OWNER_UNDEF = 0x02,
    }Owner;
    typedef enum {
        Type_Message = 0x00,
        Type_SYStem = 0x01,
    }Type;

public:
    bool PathSet(const std::string& newpath);
    std::string Now_time();
    std::wstring Now_timeW();
    std::string Days();
    template<typename CharT>
    std::basic_ostream<CharT>& getOutputStream()
    {
        if constexpr (std::is_same_v<CharT, char>)
        {
            return std::cout;
        }
        else if constexpr (std::is_same_v<CharT, wchar_t>)
        {
            return std::wcout;
        }
    }

    template<typename CharT>
    int inTypecheck()
    {
        if constexpr (std::is_same_v<CharT, char>)
            return _STR;
        else if constexpr (std::is_same_v<CharT, wchar_t>)
            return _WSTR;
    }

    template<typename CharT>
    std::basic_string<CharT> convert_to(const std::string& str);

    template<>
    std::basic_string<char> convert_to<char>(const std::string& str) {
        return str;
    }

    template<>
    std::basic_string<wchar_t> convert_to<wchar_t>(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.from_bytes(str);
    }


    template<typename CharT>
    Botlog::Logstat Record(Botlog::Type type, Botlog::Level lev, Botlog::Owner own, std::basic_string<CharT>& msg)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);  // RAII
        std::string infopath = this->logpath + Days() + ".log";
        alignas(16) std::basic_ostream<CharT>& out = getOutputStream<CharT>();
        alignas(16) std::basic_ofstream<CharT> file2(infopath, std::ios::app);
        alignas(16)std::ofstream file(infopath, std::ios::app);
        //const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
        // cmd ptint
        std::cout
            << sys_section[type]
            << lev_section[lev]
            << own_section[own]
                << Now_time()
                    << _MIDCUT;
                out << msg << std::endl;

                if (file.is_open())
                {
                    //file.imbue(utf8_locale);
                    file << sys_section[type]
                    << lev_section[lev]
                    << own_section[own]
                    << Now_time()
                    << _MIDCUT << std::flush;
                        //file2.imbue(utf8_locale);
                    file2 << msg << std::endl;
                    file2.close();
                    return LOG_WRITE_SUCCESS;
                }
                else
                {
                    return LOG_WRITE_ERR;
                }
                file2.close();
                file.close();
                return LOG_WRITE_ERR;
    }
    template<typename CharT>
    Botlog::Logstat Record(Botlog::Level lev, Botlog::Owner own, std::basic_string<CharT>& msg)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);  // RAII
        std::string infopath = this->logpath + Days() + ".log";
        alignas(16) std::basic_ostream<CharT>& out = getOutputStream<CharT>();
        alignas(16) std::basic_ofstream<CharT> file2(infopath, std::ios::app);
        alignas(16)std::ofstream file(infopath, std::ios::app);
        //const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
        // cmd ptint
        std::cout
            << _SYSTEM
            << lev_section[lev]
            << own_section[own]
            << Now_time()
            << _MIDCUT;
            out << msg << std::endl;

        if (file.is_open())
        {
            //file.imbue(utf8_locale);
            file << _SYSTEM
                << lev_section[lev]
                << own_section[own]
                << Now_time()
                << _MIDCUT << std::flush;
                //file2.imbue(utf8_locale);
                file2 << msg << std::endl;
                file2.close();
            return LOG_WRITE_SUCCESS;
        }
        else
        {
            return LOG_WRITE_ERR;
        }
        file2.close();
        file.close();
        return LOG_WRITE_ERR;
    }

    Botlog::Logstat Record(Botlog::Level lev, Botlog::Owner own, const char* msg)
    {
        return Record(lev, own, std::string(msg)); // std::string 
    }

    Botlog::Logstat Record(Botlog::Type type, Botlog::Level lev, Botlog::Owner own, const char* msg)
    {
        return Record(type, lev, own, std::string(msg)); // std::string 
    }
    Botlog::Logstat Record(Botlog::Type type, Botlog::Level lev, Botlog::Owner own, const wchar_t* msg)
    {
        return Record(type, lev, own, std::wstring(msg)); // std::string 
    }
    // const wchar_t*
    Botlog::Logstat Record(Botlog::Level lev, Botlog::Owner own, const wchar_t* msg)
    {
        return Record(lev, own, std::wstring(msg)); // std::wstring 
    }

    template<typename CharT>
    Botlog::Logstat cmdlog_inline(Botlog::Level lev, Botlog::Owner own, const std::basic_string<CharT>& msg)
    {
        alignas(16) std::basic_ostream<CharT>& out = getOutputStream<CharT>();
        const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());

        std::ostringstream oss;
        oss << _INFO
            << lev_section[lev]
            << own_section[own]
            << Now_time()
            << _MIDCUT
            << msg;

            std::string final_line = oss.str();

            std::cout << "\r" << final_line << std::string(120 - final_line.length(), ' ') << std::flush;

            return LOG_WRITE_SUCCESS;
    }

    template<typename CharT>
    Botlog::Logstat cmdlog(Botlog::Level lev, Botlog::Owner own, std::basic_string<CharT>& msg)
    {
        alignas(16) std::basic_ostream<CharT>& out = getOutputStream<CharT>();
        const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
        // cmd ptint
        std::cout
            << _INFO
            << lev_section[lev]
            << own_section[own]
            << Now_time()
            << _MIDCUT;
        out << msg << std::endl;
        return LOG_WRITE_SUCCESS;
    }

    Botlog::Logstat filelog(Botlog::Level lev, Botlog::Owner own, const char* msg)
    {
        return filelog(lev, own, std::string(msg)); // std::string 
    }

    // const wchar_t*
    Botlog::Logstat filelog(Botlog::Level lev, Botlog::Owner own, const wchar_t* msg)
    {
        return filelog(lev, own, std::wstring(msg)); // std::wstring 
    }

    template<typename CharT>
    Botlog::Logstat filelog(Botlog::Level lev, Botlog::Owner own, std::basic_string<CharT>& msg)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);

        std::string infopath = this->logpath + Days() + ".log";
        std::basic_ofstream<CharT> file(infopath, std::ios::app);

        if (!file.is_open())
        {
            return LOG_WRITE_ERR;
        }

        // UTF-8 for wchar_t
        if constexpr (std::is_same_v<CharT, wchar_t>)
        {
            file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        }

        file << convert_to<CharT>(_INFO)
            << convert_to<CharT>(lev_section[lev])
            << convert_to<CharT>(own_section[own])
            << convert_to<CharT>(Now_time())
            << convert_to<CharT>(_MIDCUT)
            << msg << std::endl;

        return LOG_WRITE_SUCCESS;
    }

private:
    bool isFileExists_fopen(std::string& name);
private:
    std::string logpath = "./log/";


};
