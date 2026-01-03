#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace Utils {
class CThreadPool {
public:
    CThreadPool(std::size_t num_threads = std::thread::hardware_concurrency());
    ~CThreadPool();

    void EnqueueJob(std::function<void()> task);
    void WaitForCompletion();

private:
    std::vector<std::thread> mThreads;
    std::queue<std::function<void()>> mTasks;
    std::mutex mTasksMutex;
    std::condition_variable mCondition;
    std::condition_variable mCompletionCondition;
    std::atomic<bool> mShouldStop{false};
    std::atomic<std::size_t> mPendingTasks{0};
};
} // namespace Utils
