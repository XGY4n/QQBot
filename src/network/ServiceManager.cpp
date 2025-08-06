#include "network/ServiceManager.h"

ServiceManager::ServiceManager(std::string reportUrl)
{
    // Constructor implementation here
}

ServiceManager::ServiceManager()
{
    _logger->LOG_SUCCESS_SELF("Initializing ServiceManager...");

    try 
    {
        _resultServer = std::make_unique<ResultHttpServer>();
        _logger->LOG_SUCCESS_SELF("ResultHttpServer initialized.");

        _running = true;

        _monitorThread = std::thread(&ServiceManager::MonitorTasks, this);
        _monitorThread.detach();
        _logger->LOG_SUCCESS_SELF("Monitor thread started.");
		_heartbeatTask = std::make_unique<HeartbeatTask>();
        _boardcastTask = std::make_unique<BoardcastTask>();
        EventBusInstance::instance().subscribe<HeartbeatHttpCb>(
            [this](const HeartbeatHttpCb& event) {
                try {
                    UpdateTaskrevTime(event.body);
                }
                catch (const std::exception& e) {
                    _logger->LOG_ERROR_SELF("Exception in HeartbeatHttpCb handler: " + std::string(e.what()));
                }
            });

        _logger->LOG_SUCCESS_SELF("Event subscription complete.");

    }
    catch (const std::exception& e) {
        _logger->LOG_ERROR_SELF("Exception during ServiceManager initialization: " + std::string(e.what()));
        throw; 
    }

    _logger->LOG_SUCCESS_SELF("ServiceManager initialized successfully.");
}

ServiceManager::~ServiceManager()
{

}

void ServiceManager::KillDuplicateTask(const HttpTaskInfo& task, TaskHash hash)
{
    KillProcess(task.pId);
    _logger->LOG_ERROR_SELF(
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
        _logger->LOG_SUCCESS_SELF("ServiceManager::start() called. Starting HTTP server thread...");
        _resultServer->setupRoutes();  
        _resultServer->start();
        _resultServer->setReportPostCallback([this](const std::string& body) {
            HandleTaskRev(body);
        });
        _logger->LOG_SUCCESS_SELF("HTTP server started successfully.");
        _logger->LOG_SUCCESS_SELF("Starting HTTP client HeartbeatTask thread...");
        _heartbeatTask->start();
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
    // 在停止服务器时记录日志
    _logger->LOG_SUCCESS_SELF("ServiceManager::stop() called. Attempting to stop HTTP server...");
    if (server_ptr) {
        server_ptr->stop(); // 停止 httplib 服务器
        _logger->LOG_SUCCESS_SELF("httplib server stopped.");
    }
    if (server_thread.joinable()) {
        server_thread.join(); // 等待线程完成
        _logger->LOG_SUCCESS_SELF("HTTP server thread joined.");
    }
    else {
        _logger->LOG_SUCCESS_SELF("HTTP server thread was not joinable (possibly detached or already finished).");
    }
    if (!_running) return;
    _logger->LOG_SUCCESS_SELF("Stopping ServiceManager monitor thread...");

    _running = false; // 通知线程退出

    if (_monitorThread.joinable()) {
        _monitorThread.join(); // 等待线程结束
    }
    _logger->LOG_SUCCESS_SELF("ServiceManager::stop() finished.");

}

void ServiceManager::RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask)
{
    _logger->LOG_WARNING_SELF("RegisterTask called for task PID: " + std::to_string(ServiceTask.pId) + " and name: " + ServiceTask.task_uuid);

    HttpTaskInfo httpTask = CreateHttpTask(ServiceTask);

    if (IsLongTask(httpTask)) {
        if (!RegisterLongTask(httpTask)) {
            KillDuplicateTask(httpTask, httpTask.hash);
            return;
        }
    }

    _TaskMapping.insert({ ServiceTask.task_uuid, httpTask });
    _logger->LOG_SUCCESS_SELF("Inserted task: task_uuid : " + ServiceTask.task_uuid + " Function JSON : " + ServiceTask.taskcallback.Jsonstring);
}

//void ServiceManager::RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask)
//{
//    _logger->LOG_WARNING_SELF("RegisterTask called for task PID: " + std::to_string(ServiceTask.pId) + " and name: " + ServiceTask.task_uuid);
//	HttpTaskInfo httpTask;
//	httpTask.callback = ServiceTask;
//	httpTask.pId = ServiceTask.pId;
//	httpTask.task_uuid = ServiceTask.task_uuid;
//	httpTask.status = ServiceTask.status;
//	httpTask.reportUrl = ServiceTask.reportUrl;
//    httpTask.returnType = ServiceTask.returnType;
//    httpTask.heartbeatPort = ServiceTask.heartbeat_port;
//    httpTask.lastHeartbeatTime = std::chrono::steady_clock::now();
//    httpTask.taskBuildTime = httpTask.lastHeartbeatTime;
//    httpTask.callInfo = ServiceTask.callInfo;
//	httpTask.taskType = ServiceTask.taskcallback.TaskType == 0 ? Long : Short;
//    if (ServiceTask.taskcallback.isUnique && httpTask.taskType == Long)//唯一任务 : 整个机器人生命周期内同一时刻只存在一个的任务 与指令无关 与文件名称强相关
//    {
//        TaskHash hash_value = std::hash<std::string>{}
//            (ServiceTask.taskcallback.pythonScriptPath +
//            ServiceTask.taskcallback.fileName + 
//            ServiceTask.taskcallback.TaskName);//唯一性校验为 节名称 + 文件路径 + 文件名称 设计上一个文件对应一个功能 不允许一个文件调用两个不同函数的情况出现
//
//        httpTask.hash = hash_value;
//        auto result = _uniqueTaskMapping.insert({ hash_value, ServiceTask.taskcallback });
//        if (!result.second) {
//            KillProcess(ServiceTask.pId);
//            _logger->LOG_ERROR_SELF(
//                "Duplicate Unique long task detected! KILL\n"
//                "  -> Hash: " + std::to_string(hash_value) + "\n"
//                "  -> Function JSON: " + ServiceTask.taskcallback.Jsonstring + "\n"
//                "  -> Process ID terminated: " + std::to_string(ServiceTask.pId)
//            );
//            return;
//        }
//    }
//	else if(httpTask.taskType == Long)// 非唯一任务 : 允许同一时刻存在多个同名任务 但是对于长任务 xxx.yyy 需要同时校验xxx.yyy 如果xxx.yyy 存在 xxx.zzz还可以拉起来
//    {
//        TaskHash hash_value = std::hash<std::string>{}
//            (ServiceTask.taskcallback.pythonScriptPath +
//            ServiceTask.taskcallback.fileName +
//            ServiceTask.taskcallback.TaskName+ ServiceTask.taskcallback.commandToRun);
//        httpTask.hash = hash_value;
//        auto result = _longTaskMapping.insert({ hash_value, ServiceTask.taskcallback });
//        if (!result.second) {
//            KillProcess(ServiceTask.pId);
//            _logger->LOG_ERROR_SELF(
//                "Duplicate long task detected! KILL\n"
//                "  -> Hash: " + std::to_string(hash_value) + "\n"
//                "  -> Function JSON: " + ServiceTask.taskcallback.Jsonstring + "\n"
//                "  -> Process ID terminated: " + std::to_string(ServiceTask.pId)
//            );
//            return;
//        }
//    }
//
//    // 短任务直接执行
//
//    _TaskMapping.insert({ ServiceTask.task_uuid, httpTask });
//    _logger->LOG_SUCCESS_SELF("Inserted task: task_uuid : " + ServiceTask.task_uuid
//        + "Function JSON : " + ServiceTask.taskcallback.Jsonstring);
//}

bool GetProcessHANDLE(DWORD pid, HANDLE& pHANDLE)
{
    // 需要 PROCESS_TERMINATE 权限才能终止进程
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
        _logger->LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is still active. Attempting to terminate.");
        if (!TerminateProcess(pHANDLE, 1))
        {
            _logger->LOG_ERROR_SELF("Failed to terminate PID " + std::to_string(processId) + ". Error: " + std::to_string(GetLastError()));
        }
        CloseHandle(pHANDLE);
    }
    else
    {
        _logger->LOG_SUCCESS_SELF("PID " + std::to_string(processId) + " is not active or could not be opened. Assuming it's done.");
    }
}

void ServiceManager::ReleaseTask(std::string uuid)
{
    auto TaskInfo = _TaskMapping.find(uuid);

    if (TaskInfo == _TaskMapping.end())
    {
        _logger->LOG_ERROR_SELF("Task with uuid " + uuid + " not found.");
        return;
    }
	KillProcess(TaskInfo->second.pId);
    _uniqueTaskMapping.erase(TaskInfo->second.hash);
    _longTaskMapping.erase(TaskInfo->second.hash);
    _logger->LOG_SUCCESS_SELF("Released task uuid : " + uuid);
}


void ServiceManager::HandleTaskRev(std::string body)
{
    nlohmann::json j = nlohmann::json::parse(body);
    std::string uuid = j["task_uuid"].get<std::string>();
    std::string taskStatus = j["status"].get<std::string>();
    auto sendBackCopy = _TaskMapping.find(uuid);
     
    if (taskStatus == "faild")
    {
        _logger->LOG_SUCCESS_SELF("HandleTaskRev ERROR : " + "status -> faild");
        return;
    }

    if (taskStatus == "success")
    {    
        if (sendBackCopy == _TaskMapping.end())
        {
            _logger->LOG_SUCCESS_SELF("HandleTaskRev ERROR : " + uuid + "empty Task");
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
                    _logger->LOG_WARNING_SELF("Task " + it->second.task_uuid + 
                        " (pid=" + std::to_string(it->second.pId) + ") is unresponsive. Killing...");

                    ReleaseTask(it->second.task_uuid);
                    it = _TaskMapping.erase(it);
                    continue;
                }
                else 
                {
                    _heartbeatTask->sendHeartbeat(it->second.task_uuid, it->second.heartbeatPort);
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
