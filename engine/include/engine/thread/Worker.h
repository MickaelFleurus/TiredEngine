#pragma once
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>

namespace Thread {
class CWorkerHandler;

class CWorker {
public:
    CWorker(CWorkerHandler& communication, std::condition_variable& condition,
            std::mutex& mutex);
    ~CWorker();

    void Start();
    std::size_t QueuedTasksCount();
    void EnqueueTask(std::function<void()> task);

    void Stop();

    std::optional<std::function<void()>> Steal();

    void Join();

private:
    std::thread mThread;
    std::deque<std::function<void()>> mTasks;
    std::mutex& mConditionMutex;
    CWorkerHandler& mCommunication;
    std::atomic<bool> mShouldStop{false};
    std::condition_variable& mGlobalCondition;
    std::mutex mLocalMutex;
};
} // namespace Thread
