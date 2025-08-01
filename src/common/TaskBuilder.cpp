﻿#include <TaskBuilder.h>

std::map<std::string, TaskBuilder::PyReturnType> TaskBuilder::ReturnTypeMap = {
    {"str", TaskBuilder::STRING},
    {"wstr", TaskBuilder::WSTRING},
    {"file", TaskBuilder::FILE},
    {"QImage", TaskBuilder::QIMAGE},
    {"Auto", TaskBuilder::AUTO},
    {"undefine", TaskBuilder::UNKNOWN}
};

std::map<std::string, TaskBuilder::KeyValue> TaskBuilder::keyMap = {
    {"Py_Call_Head", TaskBuilder::Py_Call_Head},
    {"Py_Call_Path", TaskBuilder::Py_Call_Path},
    {"Py_Call_File", TaskBuilder::Py_Call_File},
    {"Py_Call_Func", TaskBuilder::Py_Call_Fun},
    {"Py_Return_type", TaskBuilder::Py_Return_type}
};

TaskBuilder::TaskBuilder()
{
    _PyCfg = std::make_unique<WinInIWrapper>("./ini/PythonTask.ini");
    ReadPyTaskConfig();
    _taskConfigWatcherThread = std::thread(&TaskBuilder::watchTaskConfigHotReload, this, "./ini", "PythonTask.ini");

}

TaskBuilder::~TaskBuilder()
{
    _watchTaskConfigStopFlag = true;
    if (_taskConfigWatcherThread.joinable())
        _taskConfigWatcherThread.join();
}


void TaskBuilder::watchTaskConfigHotReload(const std::string& dir, const std::string& filename)
{
    std::wstring wdir = std::wstring(dir.begin(), dir.end());
    std::wstring wfile = std::wstring(filename.begin(), filename.end());

    HANDLE hDir = CreateFileW(
        wdir.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    if (hDir == INVALID_HANDLE_VALUE)
    {
        _logger->LOG_ERROR_SELF("Failed to open directory handle for config watch.");
        return;
    }

    char buffer[1024];
    DWORD bytesReturned;

    while (!_watchTaskConfigStopFlag)
    {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            nullptr,
            nullptr))
        {
            FILE_NOTIFY_INFORMATION* notify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
            std::wstring changedFile(notify->FileName, notify->FileNameLength / sizeof(WCHAR));
            if (changedFile == wfile)
            {
                _logger->LOG_SUCCESS_SELF("PythonTask.ini HotReload");
                ReadPyTaskConfig();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    CloseHandle(hDir);
}

void TaskBuilder::ReadPyTaskConfig()
{
    this->_result = this->_PyCfg->ReadAllA();
    for (const auto& iniSection : this->_result)
    {
        if (iniSection.parameters.size() != 5)
        {
			_logger->LOG_WARNING_SELF(" Section: [" + iniSection.section +
                "] expects 5 parameters, but got " +
                std::to_string(iniSection.parameters.size()) + ".");
            continue;
        }
        for (const auto& keyValue : iniSection.parameters)
        {
            if (keyValue.first.compare("Py_Call_Head") == 0)
            {
                _reflexmap.insert(std::make_pair(keyValue.second, iniSection));
            }
            _logger->LOG_SUCCESS_SELF(keyValue.first + "=>" + keyValue.second);
        }
    }
}

bool TaskBuilder::formatMessage(std::pair<std::string, std::string>& result, const std::string& input)
{
    size_t pos = input.find('.');
    if (pos == std::string::npos || pos == input.length() - 1)
        return false;

    std::string callHead = input.substr(0, pos + 1);    
    std::string callExpr = input.substr(pos + 1);       

    result = std::make_pair(callHead, callExpr);
    return true;
}
bool TaskBuilder::checkInputFormat(const std::string& input, std::string& errMsg) 
{
    if (input.empty()) {
        errMsg = "Input message is empty.";
        return false;
    }

    if (input == ".") {
        errMsg = "Input message is a single dot.";
        return false;
    }

    if (input.front() == '.') {
        errMsg = "Input message starts with a dot.";
        return false;
    }

    if (input.back() == '.') {
        errMsg = "Input message ends with a dot.";
        return false;
    }

    if (input.find('.') == std::string::npos) {
        errMsg = "Input message does not contain a dot separator.";
        return false;
    }

    return true;
}

TaskBuilder::PyReflexCallInfo TaskBuilder::analysisPyReflexCallInfo()
{
    TaskBuilder::PyReflexCallInfo analysis;
    for (const auto& keyValue : _currheadMapping.parameters)
    {
        switch (keyMap[keyValue.first])
        {
        case Py_Call_Head:
            analysis.callHead = keyValue.second;
            break;
        case Py_Call_Path:
            analysis.callPath= keyValue.second;
            break;
        case Py_Call_File:
            analysis.callFile = keyValue.second;
            break;
        case Py_Call_Fun:
            analysis.callFunc = keyValue.second;
            break;
        case Py_Return_type:
            analysis.returnType = keyValue.second;
            break;
        }
    }
    return analysis;
}

bool TaskBuilder::GetcurrMapping(const std::string& callHead)
{
	auto it = _reflexmap.find(callHead);
	if (it != _reflexmap.end()) 
    {
		_currheadMapping = it->second;
		return true;
	}
	else 
    {
		_logger->LOG_ERROR_SELF("Call head not found: " + callHead);
		return false;
	}
}

Task TaskBuilder::build(const QMessage rawMessage) 
{
    Task task;

    const std::string& inputMsg = rawMessage.message;
    std::string errMsg;

    if (!checkInputFormat(inputMsg, errMsg)) {
        _logger->LOG_ERROR_SELF("formatMessage failed with: " + errMsg);
        task.status = false;
        return task;
    }
    std::pair<std::string, std::string> parsedResult;
    if (!formatMessage(parsedResult, inputMsg)) 
    {
        _logger->LOG_ERROR_SELF("Failed to parse message."
            "   Tuple Result:"
            "   First : [" + parsedResult.first + "]"
            "   Second: [" + parsedResult.second + "]");
        task.status = false;
        return task;
    }
    _logger->LOG_SUCCESS_SELF("Success to parse message."
        "  Tuple Result:"
        "  First : [" + parsedResult.first + "]"
        "  Second: [" + parsedResult.second + "]");

    const std::string& header = parsedResult.first;
    std::string& expr = parsedResult.second;
	task.commandToRun = expr;
    if (!GetcurrMapping(header))
    {
		_logger->LOG_ERROR_SELF("GetcurrMapping failed for header: " + header);
        task.status = false;
		return task;
    }
    PyReflexCallInfo reflexinfo = analysisPyReflexCallInfo();
	task.pythonScriptPath = reflexinfo.callPath;
    task.messageId = next_id();
	task.status = true;
	task.functionName = reflexinfo.callFunc;
	task.fileName = reflexinfo.callFile;
	task.argument = expr;
    task.callInfo = rawMessage;
    auto test = expr.c_str();
    std::error_code ec;
    std::filesystem::path abs_path = std::filesystem::absolute(task.pythonScriptPath, ec);
    if (ec)
    {
        task.status = false;
        _logger->LOG_ERROR_SELF("Failed to get absolute path from script path: " + task.pythonScriptPath + ", error: " + ec.message());
        return task;
    }
    nlohmann::json j;
    j["Type"] = multi_Byte_To_Wide_Char2(rawMessage.message);
    j["value"] = multi_Byte_To_Wide_Char2(test);
    j["script_path"] = abs_path;
    j["Module"] = task.fileName;
	j["Function"] = task.functionName;
    j["ReturnType"] = reflexinfo.returnType;
    task.Jsonstring = j.dump();
    return task;
}