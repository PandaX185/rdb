#pragma once
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>

namespace pool
{
    class ThreadPool
    {
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;

        std::mutex queue_mutex_;
        std::condition_variable condition_;
        bool stop_;

    public:
        ThreadPool(size_t num_threads);
        ~ThreadPool();

        void enqueue(const std::function<void()> &task);
    };
}
