#include <TaskRunner.h>
#include <Python.h>
#include <nlohmann/json.hpp>

PythonTaskRunner::PythonTaskRunner()
{
    ReadPythonEnvConfig();
    Py_SetPythonHome(_pythonHomeW.c_str());
    LOG_SUCCESS_SELF("Python Interpreter initialization successful！");
}

PythonTaskRunner::~PythonTaskRunner()
{
    if (Py_IsInitialized()) {
        Py_Finalize();
    }
}
 
void PythonTaskRunner::ReadPythonEnvConfig()
{
    _PyenvCfg = new WinInIWrapper(L"./ini/PythonEnv.ini");
    _pythonHomeW = std::filesystem::absolute(_PyenvCfg->FindValueW<std::wstring>(L"PYTHONHOME", L"HOME"));
    _pythonHomeA = std::string(_pythonHomeW.begin(), _pythonHomeW.end());
}

std::optional<PythonTaskRunner::ServiceCallbackInfo> PythonTaskRunner::run(Task task)
{
    const char* ans_string = nullptr;
    if (!Py_IsInitialized()) {
        Py_Initialize();
    }    
    try           
    {    
        PyRun_SimpleString("import sys"); 

        std::string pathCommand = "sys.path.append('./Python')";
        PyRun_SimpleString(pathCommand.c_str());
        
        PyObject* pModule = PyImport_ImportModule("dispatcher");
        if (!pModule) {
            PyErr_Print();
            throw std::runtime_error("无法加载模块");
        } 
         
        PyObject* pFunc = PyObject_GetAttrString(pModule, "main");
        if (!pFunc || !PyCallable_Check(pFunc)) {
            PyErr_Print();
            Py_XDECREF(pModule);
            throw std::runtime_error("无法找到函数");
        }
        
        PyObject* arg1 = PyUnicode_FromString(task.Jsonstring.c_str());
        PyObject* arg2 = PyUnicode_FromString(_pythonHomeA.c_str());
        PyObject* pArg = PyTuple_Pack(2, arg1, arg2);
        Py_DECREF(arg1);
        Py_DECREF(arg2);

        if (!pArg) {
            Py_XDECREF(pFunc);
            Py_XDECREF(pModule);
            PyErr_Print();
            throw std::runtime_error("参数构造失败！");
        }  
           
        PyObject* pResult = PyObject_CallObject(pFunc, pArg);
        Py_DECREF(pArg);
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);

        if (!pResult) {
            PyErr_Print();
            throw std::runtime_error("函数调用失败！");
        }

        if (!PyUnicode_Check(pResult)) {
            Py_DECREF(pResult);
            throw std::runtime_error("Python 函数返回值不是字符串");
        }

        ans_string = PyUnicode_AsUTF8(pResult);
        Py_DECREF(pResult);
    }
    catch (const std::exception& e)  
    {
        LOG_ERROR_SELF("Python task execution failed: " + std::string(e.what()));
        return std::nullopt;
    }

    if (!ans_string) {
        LOG_ERROR_SELF("Python task execution returned null string");
        return std::nullopt;
    }

    LOG_SUCCESS_SELF("Python task execution done: " + std::string(ans_string));

    return BuildTCB(ans_string, task);
}

PythonTaskRunner::ServiceCallbackInfo PythonTaskRunner::BuildTCB(const char* TaskCallBackJsonstr, Task task)
{
    ServiceCallbackInfo info;
    nlohmann::json j = nlohmann::json::parse(TaskCallBackJsonstr);

    info.pId = j["pid"].get<unsigned int>();
    info.heartbeat_port = j["heartbeat_port"].get<unsigned int>();
    info.reportUrl = j["report_port"].get<std::string>();
    info.task_uuid = j["task_uuid"].get<std::string>();
    info.status = j["status"].get<short>();
    info.returnType = ReturnTypeMap[j["return_type"].get<std::string>()];
    info.callInfo = task.callInfo;
    info.taskcallback = task;

    return info;
}
