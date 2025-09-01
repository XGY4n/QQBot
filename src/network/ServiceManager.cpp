#include "network/ServiceManager.h"

ServiceManager::ServiceManager(std::string reportUrl)
{
    // Constructor implementation here
}

ServiceManager::ServiceManager()
{
    LOG_SUCCESS_SELF("Initializing ServiceManager...");

    try 
    {
        _resultServer = std::make_unique<NetWorkResultServer>();
        LOG_SUCCESS_SELF("ResultHttpServer initialized.");

        _running = true;

        _monitorThread = std::thread(&ServiceManager::MonitorTasks, this);
        _monitorThread.detach();
        LOG_SUCCESS_SELF("Monitor thread started.");
		_heartbeatTask = std::make_unique<HeartbeatTask>();
        _boardcastTask = std::make_unique<BoardcastTask>();
        EventBusInstance::instance().subscribe<HeartbeatHttpCb>(
            [this](const HeartbeatHttpCb& event) {
                try {
                    UpdateTaskrevTime(event.body);
                }
                catch (const std::exception& e) {
                    LOG_ERROR_SELF("Exception in HeartbeatHttpCb handler: " + std::string(e.what()));
                }
            });

        LOG_SUCCESS_SELF("Event subscription complete.");

    }
    catch (const std::exception& e) {
        LOG_ERROR_SELF("Exception during ServiceManager initialization: " + std::string(e.what()));
        throw; 
    }

    LOG_SUCCESS_SELF("ServiceManager initialized successfully.");
}

ServiceManager::~ServiceManager()
{
    stop();
}

void ServiceManager::KillDuplicateTask(const HttpTaskInfo& task, TaskHash hash)
{
    KillProcess(task.pId);
    LOG_ERROR_SELF(
        "Duplicate long task detected! KILL\n"
        "  -> Hash: " + std::to_string(hash) + "\n"
        "  -> Function JSON: " + task.callback.taskcallback.Jsonstring + "\n"
        "  -> Process ID terminated: " + std::to_string(task.pId)
    );
}

ServiceManager::HttpTaskInfo ServiceManager::CreateHttpTask(const PythonTaskRunner::ServiceCallbackInfo& ServiceTask)
{
    HttpTaskInfo httpTask;
    httpTask.callback = ServiceTask;
    httpTask.pId = ServiceTask.pId;
    httpTask.task_uuid = ServiceTask.task_uuid;
    httpTask.status = ServiceTask.status;
    httpTask.reportUrl = ServiceTask.reportUrl;
    httpTask.returnType = ServiceTask.returnType;
    httpTask.heartbeatPort = ServiceTask.heartbeat_port;
    httpTask.lastHeartbeatTime = std::chrono::steady_clock::now();
    httpTask.taskBuildTime = httpTask.lastHeartbeatTime;
    httpTask.callInfo = ServiceTask.callInfo;
    httpTask.taskType = ServiceTask.taskcallback.TaskType == 0 ? Long : Short;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(httpTask.heartbeatPort); // Python �˿�
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Python IP
    connect(httpTask.taskSocket, (sockaddr*)&addr, sizeof(addr));
    return httpTask;
} 

bool ServiceManager::RegisterLongTask(HttpTaskInfo& task)
{
    TaskHash hash_value = task.callback.taskcallback.isUnique
        ? CalcUniqueTaskHash(task.callback.taskcallback)
        : CalcNormalTaskHash(task.callback.taskcallback);

    task.hash = hash_value;

    auto& targetMap = task.callback.taskcallback.isUnique ? _uniqueTaskMapping : _longTaskMapping;
    auto result = targetMap.insert({ hash_value, task.callback.taskcallback });
    return result.second;
}

ServiceManager::TaskHash ServiceManager::CalcUniqueTaskHash(const Task& cb)
{
    return std::hash<std::string>{}(cb.pythonScriptPath + cb.fileName + cb.TaskName);
}

ServiceManager::TaskHash ServiceManager::CalcNormalTaskHash(const Task& cb)
{
    return std::hash<std::string>{}(cb.pythonScriptPath + cb.fileName + cb.TaskName + cb.commandToRun);
}

bool ServiceManager::IsLongTask(const HttpTaskInfo& task) const
{
    return task.taskType == Long;
}

void ServiceManager::start()
{            
    try
    {    
        LOG_SUCCESS_SELF("ServiceManager::start() called. Starting HTTP server thread...");
        _resultServer->setupRoutes();  
        _resultServer->start();
        _resultServer->setReportPostCallback([this](const std::string& body) {
            HandleTaskRev(body);
        });
        LOG_SUCCESS_SELF("HTTP server started successfully.");
        LOG_SUCCESS_SELF("Starting HTTP client HeartbeatTask thread...");
        _heartbeatTask->start();
        LOG_SUCCESS_SELF("HTTP server started successfully.");
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR_SELF(std::string("Exception in ServiceManager::start(): ") + ex.what());
    }
    catch (...)
    {
        LOG_ERROR_SELF("Unknown exception in ServiceManager::start()");
    }
}


void ServiceManager::stop()
{
    // ��ֹͣ������ʱ��¼��־
    LOG_SUCCESS_SELF("ServiceManager::stop() called. Attempting to stop HTTP server...");
    if (server_ptr) {
        server_ptr->stop(); // ֹͣ httplib ������
        LOG_SUCCESS_SELF("httplib server stopped.");
    }
    if (server_thread.joinable()) {
        server_thread.join(); // �ȴ��߳����
        LOG_SUCCESS_SELF("HTTP server thread joined.");
    }
    else {
        LOG_SUCCESS_SELF("HTTP server thread was not joinable (possibly detached or already finished).");
    }
    if (!_running) return;
    LOG_SUCCESS_SELF("Stopping ServiceManager monitor thread...");

    _running = false; // ֪ͨ�߳��˳�

    if (_monitorThread.joinable()) {
        _monitorThread.join(); // �ȴ��߳̽���
    }
    LOG_SUCCESS_SELF("ServiceManager::stop() finished.");

}

void ServiceManager::RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask)
{
    LOG_WARNING_SELF("RegisterTask called for task PID: " + std::to_string(ServiceTask.pId) + " and name: " + ServiceTask.task_uuid);

    HttpTaskInfo httpTask = CreateHttpTask(ServiceTask);

    if (IsLongTask(httpTask)) {
        if (!RegisterLongTask(httpTask)) {
            KillDuplicateTask(httpTask, httpTask.hash);
            return;
        }
    }

    _TaskMapping.insert({ ServiceTask.task_uuid, std::move(httpTask)});
    LOG_SUCCESS_SELF("Inserted task: task_uuid : " + ServiceTask.task_uuid + " Function JSON : " + ServiceTask.taskcallback.Jsonstring);
}

bool GetProcessHANDLE(DWORD pid, HANDLE& pHANDLE)
{
    // ��Ҫ PROCESS_TERMINATE Ȩ�޲�����ֹ����
    pHANDLE = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pid);
    if (!pHANDLE)
    {
        return false;
    }
    return true;
}

void ServiceManager::KillProcess(DWORD processId)
{
    HANDLE pHANDLE = NULL;
    if (GetProcessHANDLE(processId, pHANDLE))
    {
        LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is still active. Attempting to terminate.");
        if (!TerminateProcess(pHANDLE, 1))
        {
            LOG_ERROR_SELF("Failed to terminate PID " + std::to_string(processId) + ". Error: " + std::to_string(GetLastError()));
        }
        CloseHandle(pHANDLE);
    }
    else
    {
        LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is not active or could not be opened. Assuming it's done.");
    }
}

void ServiceManager::ReleaseTask(std::string uuid)
{
    auto TaskInfo = _TaskMapping.find(uuid);

    if (TaskInfo == _TaskMapping.end())
    {
        LOG_ERROR_SELF("Task with uuid " + uuid + " not found.");
        return;
    }
	KillProcess(TaskInfo->second.pId);
    _uniqueTaskMapping.erase(TaskInfo->second.hash);
    _longTaskMapping.erase(TaskInfo->second.hash);
    LOG_SUCCESS_SELF("Released task uuid : " + uuid);
}


void ServiceManager::HandleTaskRev(std::string body)
{
    nlohmann::json j = nlohmann::json::parse(body);
    std::string uuid = j["task_uuid"].get<std::string>();
    std::string taskStatus = j["status"].get<std::string>();
    auto sendBackCopy = _TaskMapping.find(uuid);
     
    if (taskStatus == "faild")
    {
        LOG_SUCCESS_SELF("HandleTaskRev ERROR : " + "status -> faild");
        return;
    }

    if (taskStatus == "success")
    {    
        if (sendBackCopy == _TaskMapping.end())
        {
            LOG_SUCCESS_SELF("HandleTaskRev ERROR : " + uuid + "empty Task");
            return;
        }
        ReleaseTask(uuid);
        EventBusInstance::instance().publish(HttpCallbackInfo{ body, sendBackCopy->second.callInfo });
        return;
    }  
    EventBusInstance::instance().publish(HttpCallbackInfo{ body });
}   
 

void ServiceManager::MonitorTasks() 
{
    while (_running) 
    {
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard<std::mutex> lock(_taskMapMutex);
            for (auto it = _TaskMapping.begin(); it != _TaskMapping.end(); ) 
            {
                if (it->second.taskType == Short)
                {
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        now - it->second.taskBuildTime).count();
                    if (duration > 5)
                    {
                        ReleaseTask(it->second.task_uuid);
                        it = _TaskMapping.erase(it);
                        continue;
                    }
                }
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                    now - it->second.lastHeartbeatTime).count();
                if (duration > 3) 
                {   
                    LOG_WARNING_SELF("Task " + it->second.task_uuid + 
                        " (pid=" + std::to_string(it->second.pId) + ") is unresponsive. Killing...");

                    ReleaseTask(it->second.task_uuid);
                    it = _TaskMapping.erase(it);
                    continue;
                }
                else 
                {
                    _heartbeatTask->sendHeartbeatTCP(it->second.taskSocket, it->second.heartbeatPort);
                }
                ++it;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ServiceManager::UpdateTaskrevTime(std::string body)
{
    nlohmann::json j = nlohmann::json::parse(body);
    std::string uuid = j["uuid"].get<std::string>();
    auto it = _TaskMapping.find(uuid);
    if (it != _TaskMapping.end()) {
        it->second.lastHeartbeatTime = std::chrono::steady_clock::now();
    }
}

void ServiceManager::PostBoardcast(QMessage msg)
{
    _boardcastTask->BoardcastMessage(msg);
}
