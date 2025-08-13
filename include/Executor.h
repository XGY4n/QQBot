#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <TaskBuilder.h>
#include <Botlog.h>
#include <functional>
#include "QMsgFormatter.h"
#include "QQMessageSender.h"
#include <network/ServiceManager.h>
template<typename T>
class Executor {
public:
    Executor()
    {
        _pyTaskBuilder = std::make_unique<TaskBuilder>();
        _pyTaskrunner = std::make_unique<PythonTaskRunner>();
		_serviceManager = std::make_unique<ServiceManager>(); 
        _sender = std::make_unique<QQMessageSender>(_targetGroup);
		if (!_serviceManager) {
			LOG_ERROR_SELF("Failed to create ServiceManager instance.");
			return;
		}        
        EventBusInstance::instance().subscribe<HttpCallbackInfo>(
            [this](const HttpCallbackInfo& event) {
                LOG_SUCCESS_SELF("send to QQ :" + event.HttpBody);
                _sender->sendMessageAsJson(event.HttpBody, event.callInfo);//sendMessageAsJson
            });
        EventBusInstance::instance().subscribe<TaskConfigReloadEvent>(
            [this](const TaskConfigReloadEvent& event) {
                SetupAutoStart();
            });
		_serviceManager->start(); // Start the service manager to handle tasks
        SetupAutoStart();
    } 

    ~Executor() 
    { 
        stop(); 
    }  

    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(value);
        _cv.notify_one();
    }

    void push(T&& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push(std::move(value));
        _cv.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this] { return !_queue.empty(); });
        T value = std::move(_queue.front());
        _queue.pop();
        return value;
    }
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.empty()) return std::nullopt;
        T value = std::move(_queue.front());
        _queue.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }
    
    void start();
    void stop();

	std::atomic<bool> GetRunning()
    {
		return _running.load();
	}

    void SetHWDN(HWND target)
    {
        _targetGroup = target;
        _sender = move(std::make_unique<QQMessageSender>(_targetGroup));
    }

private:
    void SetupAutoStart();
    void HandleTask();  
    void SubmitTask(Task task);
    bool BuildTask(std::optional<T> task, Task& taskout);
    std::optional<T> GetNextTask();
    void ProcessSingleTask(std::optional<T> original_task_data);

    mutable std::mutex _mutex;
    std::queue<T> _queue;
    std::condition_variable _cv;
    HWND _targetGroup;
    std::unique_ptr<IQQMessageSender> _sender = std::make_unique<QQMessageSender>(_targetGroup);
    std::thread _worker;      
    std::atomic<bool> _running{ false }; 
    std::unique_ptr<TaskBuilder> _pyTaskBuilder;
    std::unique_ptr<PythonTaskRunner> _pyTaskrunner;
	std::unique_ptr<ServiceManager> _serviceManager = nullptr;
};



