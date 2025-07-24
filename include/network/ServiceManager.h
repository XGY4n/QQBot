#pragma once
//ServiceManager
// ResultHttpServer
//   HttpServerAdapter
//       httplib::Server
#include <network/HeartbeatTask.h>
#include <network/ResultHttpServer.h>
#include <memory>
#include <map>
#include <string>
#include <TaskRunner.h>
#include <thread>
#include <nlohmann/json.hpp>
#include <Bus/EventBusInstance.h>

class ServiceManager 
{
    enum Type
    {
		Long = 0, 
		Short = 1, 
    };
    struct HttpTaskInfo {
        unsigned int pId = 0;
        std::string task_uuid = "";
        short status = 0;
		std::string reportUrl = "";
		int heartbeatPort = 0;
		Type taskType = Short;
        PythonTaskRunner::PyReturnType returnType;
        std::chrono::steady_clock::time_point taskBuildTime;
        std::chrono::steady_clock::time_point lastHeartbeatTime;
        PythonTaskRunner::ServiceCallbackInfo callback; 
        QMessage callInfo;
    };

public:
    ServiceManager(std::string reportUrl);
    ServiceManager();
    ~ServiceManager();
    void start();
    void stop();
	void RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask);//PythonTaskRunner::PythonServiceCallbackInfo ServiceTask
    void HandleTaskRev(std::string body);
    void MonitorTasks();
    void UpdateTaskrevTime(std::string body);
    void ReleaseTask(std::string uuid);

private:
    Botlog* _logger = Botlog::GetInstance();
	std::map<std::string, HttpTaskInfo> _TaskMapping;
    std::unique_ptr<httplib::Server> server_ptr; 
    std::thread server_thread;
    std::unique_ptr<ResultHttpServer> _resultServer;
    std::unique_ptr<HeartbeatTask> _heartbeatTask;
    std::atomic<bool> _running{ false };
    std::mutex _taskMapMutex;
    std::thread _monitorThread;

};