#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <list>
#include <functional>

struct ThreadPoolTask
{
	bool status;
	std::thread task;

};

class ThreadPool {
    std::vector<std::thread> workers;      
    std::queue<std::function<void()>> tasks;  
    std::mutex queueMutex;
    std::condition_variable _condVar;
    bool stop = false;

public:
    void enqueue(std::function<void()> job) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            tasks.push(std::move(job));
        }
        _condVar.notify_one();
    }


    void workerLoop() {
        while (!stop) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                _condVar.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    ThreadPool(int n) {
        for (int i = 0; i < n; ++i)
            workers.emplace_back(&ThreadPool::workerLoop, this);
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        _condVar.notify_all();
        for (auto& t : workers) t.join();
    }
};