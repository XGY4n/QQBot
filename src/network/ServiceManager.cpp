#include "network/ServiceManager.h"

using namespace std;

ServiceManager::ServiceManager(std::string reportUrl)
{
    // Constructor implementation here
}

ServiceManager::ServiceManager()
{
    _resultServer = std::make_unique<ResultHttpServer>();
}

ServiceManager::~ServiceManager()
{
    // Destructor implementation here
}

void ServiceManager::start()
{    
    try
    {
        _logger->LOG_SUCCESS_SELF("ServiceManager::start() called. Starting HTTP server thread...");

        _resultServer->setupRoutes();  // ע������·��
        _resultServer->start();
        _resultServer->setReportPostCallback([this](const std::string& body) {
            ReleassTask(body);
        });
        _logger->LOG_SUCCESS_SELF("HTTP server started successfully.");
    }
    catch (const std::exception& ex)
    {
        _logger->LOG_ERROR_SELF(std::string("Exception in ServiceManager::start(): ") + ex.what());
    }
    catch (...)
    {
        _logger->LOG_ERROR_SELF("Unknown exception in ServiceManager::start()");
    }
}


void ServiceManager::stop()
{
    // ��ֹͣ������ʱ��¼��־
    _logger->LOG_SUCCESS_SELF("ServiceManager::stop() called. Attempting to stop HTTP server...");
    if (server_ptr) {
        server_ptr->stop(); // ֹͣ httplib ������
        _logger->LOG_SUCCESS_SELF("httplib server stopped.");
    }
    if (server_thread.joinable()) {
        server_thread.join(); // �ȴ��߳����
        _logger->LOG_SUCCESS_SELF("HTTP server thread joined.");
    }
    else {
        _logger->LOG_SUCCESS_SELF("HTTP server thread was not joinable (possibly detached or already finished).");
    }
    _logger->LOG_SUCCESS_SELF("ServiceManager::stop() finished.");
}

void ServiceManager::RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask)
{
    _logger->LOG_SUCCESS_SELF("RegisterTask called for task ID: " + std::to_string(ServiceTask.pId) + " and name: " + ServiceTask.task_uuid);
	HttpTaskInfo httpTask;
	httpTask.callback = ServiceTask;
	httpTask.pId = ServiceTask.pId;
	httpTask.task_uuid = ServiceTask.task_uuid;
	httpTask.status = ServiceTask.status;
	httpTask.reportUrl = ServiceTask.reportUrl;
    httpTask.returnType = ServiceTask.returnType;
    httpTask.heartbeatPort = ServiceTask.heartbeat_port;
    _TaskMapping.insert({ ServiceTask.task_uuid, httpTask});
    _logger->LOG_SUCCESS_SELF("Task with ID " + std::to_string(ServiceTask.pId) + " registered successfully.");
}

bool GetProcessHANDLE(DWORD pid, HANDLE& pHANDLE)
{
    // ��Ҫ PROCESS_TERMINATE Ȩ�޲�����ֹ����
    pHANDLE = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pid);
    if (!pHANDLE)
    {
        // ����ѡ���������¼���󣬻����õ����ߴ���
        return false;
    }
    return true;
}

void ServiceManager::ReleassTask(std::string body)
{
    nlohmann::json j = nlohmann::json::parse(body);
    std::string uuid = j["task_uuid"].get<std::string>();
    auto TaskInfo = _TaskMapping.find(uuid);

    if (TaskInfo == _TaskMapping.end())
    {
        _logger->LOG_ERROR_SELF("Task with uuid " + uuid + " not found.");
        return;
    }

    HANDLE pHANDLE = NULL;
    DWORD processId = TaskInfo->second.pId; // ��ȡ��ȷ�� PID

    if (GetProcessHANDLE(processId, pHANDLE))
    {
        // ��ӡ��ȷ�� PID
        _logger->LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is still active. Attempting to terminate.");
        if (!TerminateProcess(pHANDLE, 1))
        {
            _logger->LOG_ERROR_SELF("Failed to terminate PID " + std::to_string(processId) + ". Error: " + std::to_string(GetLastError()));
        }
        CloseHandle(pHANDLE); // �رս��̾���Ա���й©
    }
    else
    {
        _logger->LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is not active or could not be opened. Assuming it's done.");
    }

    _TaskMapping.erase(uuid);
    _logger->LOG_SUCCESS_SELF("Released task uuid : " + uuid);
}



