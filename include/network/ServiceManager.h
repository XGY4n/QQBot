#pragma once
//ServiceManager
// ResultHttpServer
//   HttpServerAdapter
//       httplib::Server
#include <network/HeartbeatTask.h>
#include <network/NetWorkResultServer.h>
#include <memory>
#include <map>
#include <string>
#include <TaskRunner.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <Bus/EventBusInstance.h>
#include <network/BoardcastTask.h>

class ServiceManager 
{
    using uuid = std::string;
    using TaskHash = size_t;
    enum TaskType
    {
		Long = 0, 
		Short = 1, 
    };
    struct HttpTaskInfo {
        unsigned int pId = 0;
        uuid task_uuid = "";
        short status = 0;
		std::string reportUrl = "";
		int heartbeatPort = 0;
		TaskType taskType = Short;
        PythonTaskRunner::PyReturnType returnType;
        std::chrono::steady_clock::time_point taskBuildTime;
        std::chrono::steady_clock::time_point lastHeartbeatTime;
        PythonTaskRunner::ServiceCallbackInfo callback; 
        QMessage callInfo;
        TaskHash hash = 0;
        std::unique_ptr<httplib::Client> healthclient;
        SOCKET taskSocket;
    };

public:
    ServiceManager(std::string reportUrl);
    ServiceManager();
    ~ServiceManager();
    void start();
    void stop();
	void RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask);
    void HandleTaskRev(std::string body);
    void MonitorTasks();
    void UpdateTaskrevTime(std::string body);
    void ReleaseTask(std::string uuid);
    void PostBoardcast(QMessage msg);

private:
    void KillProcess(DWORD processId);
    HttpTaskInfo CreateHttpTask(const PythonTaskRunner::ServiceCallbackInfo& ServiceTask);
    bool RegisterLongTask(HttpTaskInfo& task);
    void KillDuplicateTask(const HttpTaskInfo& task, TaskHash hash);
    TaskHash CalcUniqueTaskHash(const Task& cb);
    TaskHash CalcNormalTaskHash(const Task& cb);
    bool IsLongTask(const HttpTaskInfo& task) const;

private:
	std::map<uuid, HttpTaskInfo> _TaskMapping;
    std::map<TaskHash, Task> _longTaskMapping;
    std::map<TaskHash, Task> _uniqueTaskMapping;
    std::unique_ptr<httplib::Server> server_ptr; 
    std::thread server_thread;
    std::unique_ptr<NetWorkResultServer> _resultServer;
    std::unique_ptr<HeartbeatTask> _heartbeatTask;
    std::unique_ptr<BoardcastTask> _boardcastTask;

    std::atomic<bool> _running{ false };
    std::mutex _taskMapMutex;
    std::thread _monitorThread;

};