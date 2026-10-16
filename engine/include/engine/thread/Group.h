#pragma once

#include <condition_variable>
#include <mutex>

namespace Thread {
class CWorkerHandler;
class CGroup {
public:
    CGroup(std::size_t totalTasks, CWorkerHandler& workerHandler);

    void WaitForCompletion();
    void OnTaskDone();

private:
    CWorkerHandler& mWorkerHandler;
    std::condition_variable mCondition;
    std::mutex mMutex;
    std::size_t mTaskLeft;
};
} // namespace Thread
