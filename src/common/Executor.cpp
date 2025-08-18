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
        LOG_ERROR_SELF("Received an empty optional task from queue.");
        return;
    }

    Task builtTask;
    if (!BuildTask(original_task_data, builtTask)) {
        if (original_task_data.has_value()) {
            _serviceManager->PostBoardcast(*original_task_data);
        }
        return;
    }

    if (builtTask.status == 1) {
        LOG_SUCCESS_SELF("Task built successfully and is valid. Submitting Task with ID: " + std::to_string(builtTask.messageId));
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
        LOG_ERROR_SELF(error_msg);
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
    if (!taskinfo_opt || !taskinfo_opt.value().status) //if (!taskinfo_opt.has_value())
    {
        LOG_ERROR_SELF("Failed to run task: " + std::to_string(task.messageId));
        return;
    }
    _serviceManager->RegisterTask(taskinfo_opt.value());
}
template<typename T>

void Executor<T>::SetupAutoStart()
{
    for (auto task : _pyTaskBuilder->GetAutoStartTasks())
    {
        std::string sender = "TaskDispatcher";
        std::string id = std::to_string(std::hash<std::string>{}(task));
        std::string timestamp = std::to_string(std::time(nullptr));
        QMessage msg{ sender, id, task, timestamp };
        this->push(msg);
    }
}
template class Executor<QMessage>;