#pragma once
#include <functional>
#include <thread>
#include <vector>

#include "engine/thread/Group.h"
#include "engine/thread/WorkerHandler.h"

namespace Thread {
class CPool {
public:
    CPool();
    ~CPool();

    std::unique_ptr<CGroup> EnqueueJob(std::function<void()> task);
    std::unique_ptr<CGroup>
    EnqueueJobs(const std::vector<std::function<void()>>& tasks);

private:
    CWorkerHandler mWorkerHandler;
};
} // namespace Thread
