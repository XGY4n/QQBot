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
class ServiceManager 
{
    enum Type
    {
		Long = 0, 
		Short = 1, 
    };
    struct HttpTaskInfo {
        unsigned int pId;
        std::string task_uuid;
        short status;
		std::string reportUrl;
		int heartbeatPort;
		Type taskType = Short;
        PythonTaskRunner::PyReturnType returnType;
        PythonTaskRunner::ServiceCallbackInfo callback; 
    };

public:
    ServiceManager(std::string reportUrl);
    ServiceManager();
    ~ServiceManager();
    void start();
    void stop();
	void RegisterTask(PythonTaskRunner::ServiceCallbackInfo ServiceTask);//PythonTaskRunner::PythonServiceCallbackInfo ServiceTask
    void ReleassTask(std::string body);
private:
    Botlog* _logger = Botlog::GetInstance();
	std::map<std::string, HttpTaskInfo> _TaskMapping;
    std::unique_ptr<httplib::Server> server_ptr; 
    std::thread server_thread;
    std::unique_ptr<ResultHttpServer> _resultServer;

};