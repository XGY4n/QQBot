#pragma once
#include <string>
#include <QMsgFormatter.h>
#include <map>
#include <Botlog.h>
#include <WinIniWrapper.h>
#include <atomic>
#include <limits>
#include <nlohmann/json.hpp>
#include <converSTR.h>
struct Task 
{
    QMessage callInfo;
    unsigned int messageId;
    bool status = false;
    short TaskType;
    std::string TaskName;
    std::string pythonScriptPath;
    std::string commandToRun;  
	std::string functionName = "main";
	std::string fileName;
    std::string argument;
    std::string Jsonstring;
};
class TaskBuilder
{
public:
    struct PyReflexCallInfo 
    {
		short taskType = 1;
        std::string taskName;
        std::string callHead;
        std::string callPath;
        std::string callFile;
        std::string callFunc;
        std::string returnType;
    };

    enum KeyValue {
        Py_Call_Head,
        Py_Call_Path,
        Py_Call_File,
        Py_Call_Fun,
        Py_Return_type,
        Py_Long_Task
    };

    enum PyReturnType {
        STRING,
        WSTRING,
        FILE,
        QIMAGE,
        AUTO,
        UNKNOWN
    };
    static std::map<std::string, PyReturnType> ReturnTypeMap;
    static std::map<std::string, KeyValue> keyMap;

    TaskBuilder();
    ~TaskBuilder();

    Task build(const QMessage rawMessage);
    void ReadPyTaskConfig();
private:
#ifdef max
#undef max
#endif
    inline int next_id() {
        static std::atomic<int> current_id{ 0 };
        int old = current_id.fetch_add(1);
        if (old == std::numeric_limits<int>::max()) {
            current_id.store(0);  // not strictly atomic reset, but acceptable for basic use
        }
        return old;
    }
    PyReflexCallInfo analysisPyReflexCallInfo();
    void watchTaskConfigHotReload(const std::string& dir, const std::string& filename);
	bool formatMessage(std::pair<std::string, std::string>& result, const std::string& input);
    bool checkInputFormat(const std::string& input, std::string& errMsg);
    bool GetcurrMapping( const std::string& callHead);
public:
private:
    Botlog* _logger = Botlog::GetInstance();
    std::unique_ptr<WinInIWrapper> _PyCfg;
    std::vector<WinInIWrapper::InIMapping<std::string>> _result;
	WinInIWrapper::InIMapping<std::string> _currheadMapping;
    std::map<std::string, WinInIWrapper::InIMapping<std::string>> _reflexmap;
    std::atomic<bool> _watchTaskConfigStopFlag{ false };
    std::thread _taskConfigWatcherThread;
};
