#include <Executor.h>

template<typename T>
void Executor<T>::start() {
    _running = true;
    _worker = std::thread(&Executor<T>::HandleTask, this);
    _worker.join();
}

template<typename T>
void Executor<T>::stop() {
    _running = false;
    _cv.notify_all();
    if (_worker.joinable()) {
        _worker.join();
    }
}

template<typename T>
bool Executor<T>::BuildTask(std::optional<T> task, Task& taskout)
{
    if (!task.has_value())
    {
        return false;
    }
    taskout = _pyTaskBuilder->build(task.value());
    return taskout.status;
}

template<typename T>
std::optional<T> Executor<T>::GetNextTask() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this] {
        return !_queue.empty() || !_running;
        });

    if (!_running && _queue.empty()) {
        return std::nullopt;
    }

    if (!_queue.empty()) {
        std::optional<T> task = std::move(_queue.front());
        _queue.pop();
        return task;
    }
    return std::nullopt;
}

template<typename T>
void Executor<T>::ProcessSingleTask(std::optional<T> original_task_data) {
    if (!original_task_data) {
        _logger->LOG_ERROR_SELF("Received an empty optional task from queue.");
        return;
    }

    Task builtTask;
    if (!BuildTask(original_task_data, builtTask)) {
        _logger->LOG_ERROR_SELF("BuildTask Error");
        return;
    }

    if (builtTask.status == 1) {
        _logger->LOG_SUCCESS_SELF("Task built successfully and is valid. Submitting Task with ID: " + std::to_string(builtTask.messageId));
        SubmitTask(builtTask);
    }
    else {
        std::string error_msg = "Built task is invalid or not in successful status (ID: " + std::to_string(builtTask.messageId) + ").";
        if (!builtTask.status) {
            error_msg += " (Not valid)";
        }
        if (builtTask.status != 1) {
            error_msg += " (Status: " + std::to_string(builtTask.status) + ")";
        }
        _logger->LOG_ERROR_SELF(error_msg);
    }
}

template<typename T>
void Executor<T>::HandleTask() {
    while (_running) {
        std::optional<T> task_data = GetNextTask();

        if (!task_data) {
            continue;
        }
        ProcessSingleTask(task_data);
    }
}

template<typename T>
void Executor<T>::SubmitTask(Task task)
{
    std::optional<ITaskRunner::ServiceCallbackInfo> taskinfo_opt = _pyTaskrunner->run(task);
    if (!taskinfo_opt) //if (!taskinfo_opt.has_value())
    {
        _logger->LOG_ERROR_SELF("Failed to run task: " + std::to_string(task.messageId));
        return;
    }
    _serviceManager->RegisterTask(taskinfo_opt.value());
}
template class Executor<QMessage>;
//template<typename T>
//void Executor<T>::HandleTask()
//{
//    while (_running)
//    {
//        std::optional<T> task;
//        {
//            std::unique_lock<std::mutex> lock(_mutex);
//            _cv.wait(lock, [this] {
//                return !_queue.empty() || !_running;
//                });
//
//            if (!_running)
//                break;
//
//            task = std::move(_queue.front());
//            _queue.pop();
//        }
//
//        if (!task)
//        {
//            _logger->LOG_ERROR_SELF("Received null task.");
//            continue;
//        }
//
//        Task builtTask;
//        if (!BuildTask(task, builtTask))
//        {
//            _logger->LOG_ERROR_SELF("Task build failed.");
//            continue;
//        }
//
//        if (builtTask.status)
//        {
//            _logger->LOG_SUCCESS_SELF("Task built successfully. SubmitTask: " + builtTask.messageId);
//            SubmitTask(builtTask);
//        }
//        else
//        {
//            _logger->LOG_ERROR_SELF("Built task is invalid or status is false. ");
//        }
//    }
//}