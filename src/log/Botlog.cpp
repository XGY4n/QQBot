#include "Botlog.h"
//#include "SendMessage.h"

Botlog::Botlog(const std::string& logpath)
{

    this->logpath = logpath;
    if (!FolderExists(this->logpath))
        if (!CreateFolderIfNotExists(this->logpath))
            std::cout << "Botlog init error" << std::endl;
}

Botlog::~Botlog()
{

}


bool Botlog::isFileExists_fopen(std::string& name)//creat file 
{
    if (FILE* file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
}


// define a template function



bool Botlog::FileExists(const std::string& path)
{
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}


bool Botlog::CreateFileIfNotExists(const std::string& path)
{
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    CloseHandle(hFile);
    return true;
}

bool Botlog::FolderExists(const std::string& path)
{
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}


bool Botlog::CreateFolderIfNotExists(const std::string& path)
{
    BOOL result = CreateDirectoryA(path.c_str(), NULL);
    if (!result) {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS)
        {

            return false;
        }
        else
        {

            std::cout << "Failed to create folder " << path << ", error code: " << error << std::endl;
            return false;
        }
    }
    return true;
}

bool Botlog::PathSet(const std::string& newpath)
{
    this->logpath = newpath;
    if (!FolderExists(this->logpath))
        if (CreateFolderIfNotExists(this->logpath))
            return true;
    return false;

}

std::string Botlog::Now_time()
{
    auto now = std::chrono::system_clock::now();

    // 获取毫秒部分（0~999）
    uint64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);

    char strTime[30] = { 0 };

    sprintf(strTime, kmsDateTimeFormat,
        time_tm->tm_year + 1900,
        time_tm->tm_mon + 1,
        time_tm->tm_mday,
        time_tm->tm_hour,
        time_tm->tm_min,
        time_tm->tm_sec,
        static_cast<unsigned long long>(milliseconds)
    );

    return std::string(strTime);
}


std::wstring Botlog::Now_timeW()
{
    auto now = std::chrono::system_clock::now();
    std::wstring NowTime;
    uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
        - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    wchar_t strTime[25] = { 0 };
    swprintf(strTime, kDefaultDateTimeFormatW, time_tm->tm_year + kYearBase,
        time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
        time_tm->tm_min, time_tm->tm_sec);

    NowTime = strTime;

    return  NowTime ;
}


std::string Botlog::Days()
{
    auto now = std::chrono::system_clock::now();
    std::string NowTime;
    uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
        - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
    time_t tt = std::chrono::system_clock::to_time_t(now);
    auto time_tm = localtime(&tt);
    char strTime[25] = { 0 };
    sprintf(strTime, kDefaultDateTimeFormatInFilename, time_tm->tm_year + 1900,
        time_tm->tm_mon + 1, time_tm->tm_mday);
    NowTime = NowTime + strTime;
    return NowTime ;
}

//void Botlog::Temp_log(std::string& Ins)
//{
//    std::string filename = "./log/" + Days() + ".log";
//    std::ifstream input(filename);
//    if (!input.is_open()) {
//        //std::cerr << "Failed to open file " << filename << '\n';
//        Send_StringTEXT_Message("Failed to open file ");
//        return;
//    }
//
//    std::string temp_filename = "./log/temp.log";
//    std::ofstream output(temp_filename);
//    if (!output.is_open()) {
//        //std::cerr << "Failed to create file " << temp_filename << '\n';
//        Send_StringTEXT_Message("Failed to create file ");
//        return;
//    }
//
//    std::copy(std::istreambuf_iterator<char>(input),
//        std::istreambuf_iterator<char>(),
//        std::ostreambuf_iterator<char>(output));
//
//    input.close();
//    output.close();
//}