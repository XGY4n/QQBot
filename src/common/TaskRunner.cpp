#include <TaskRunner.h>
#include <Python.h>
#include <nlohmann/json.hpp>

PythonTaskRunner::PythonTaskRunner()
{
	ReadPythonEnvConfig();
	Py_Initialize();
	if (!Py_IsInitialized()) {
		_logger->LOG_ERROR_SELF("Python Interpreter initialization failed！");
		throw std::runtime_error("Python interpreter initialization failed.");
	}
	_logger->LOG_SUCCESS_SELF("Python Interpreter initialization successful！");
}

PythonTaskRunner::~PythonTaskRunner()
{
	Py_Finalize();
}

void PythonTaskRunner::ReadPythonEnvConfig()
{
	_PyenvCfg = new WinInIWrapper(L"./ini/PythonEnv.ini");
	auto pyhome = _PyenvCfg->FindValueW<std::wstring>(L"PYTHONHOME", L"HOME");
	_logger->LOG_SUCCESS_SELF("Python home path: " + std::string(pyhome.begin(), pyhome.end()));
	Py_SetPythonHome(pyhome.c_str());
}

std::optional<PythonTaskRunner::ServiceCallbackInfo> PythonTaskRunner::run(Task task)
{//call dispatcher.py
    const char* ans_string;
    int s = -1;
    try
    {
        PyRun_SimpleString("import sys");
        std::string pathCommand = "sys.path.append('./Python')";
        PyRun_SimpleString(pathCommand.c_str());

        // 加载 Python 模块
        PyObject* pModule = PyImport_ImportModule("dispatcher");
        if (!pModule) {
            PyErr_Print();
            throw std::runtime_error("无法加载模块" );
        }

        // 获取模块中的函数
        PyObject* pFunc = PyObject_GetAttrString(pModule, "main");
        if (!pFunc || !PyCallable_Check(pFunc)) {
            PyErr_Print();
            throw std::runtime_error("无法找到函数");
        }

        // 构造函数参数
        PyObject* pArg = Py_BuildValue("(s)", task.Jsonstring.c_str());
        if (!pArg) {
            PyErr_Print();
            throw std::runtime_error("参数构造失败！");
        }

        // 调用函数
        PyObject* pResult = PyObject_CallObject(pFunc, pArg);
        if (!pResult) {
            PyErr_Print();
            throw std::runtime_error("函数调用失败！");
        }
        s = PyArg_Parse(pResult, "s", &ans_string);
        // 释放资源
        Py_DECREF(pResult);
        Py_DECREF(pArg);
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
    }
    catch (const std::exception& e) 
    {
		_logger->LOG_ERROR_SELF("Python task execution failed: " + std::string(e.what()));
    }
    _logger->LOG_SUCCESS_SELF("Python task execution done: " + std::string(ans_string));
    return BuildTCB(ans_string);
}

PythonTaskRunner::ServiceCallbackInfo PythonTaskRunner::BuildTCB(const char* TaskCallBackJsonstr)
{
    ServiceCallbackInfo info;
    nlohmann::json j = nlohmann::json::parse(TaskCallBackJsonstr);
    info.pId = j["pid"].get<unsigned int>();
    info.heartbeat_port = j["heartbeat_port"].get<unsigned int>();
    info.reportUrl = j["report_port"].get<std::string>();
    info.task_uuid = j["task_uuid"].get<std::string>();
    info.status = 1;
    info.returnType = ReturnTypeMap[j["return_type"].get<std::string>()];
    return info;
}
