#include "engine/thread/WorkerHandler.h"

namespace Thread {
CWorkerHandler::CWorkerHandler(std::size_t numWorkers) {
    mWorkers.reserve(numWorkers);
    for (std::size_t i = 0; i < numWorkers; ++i) {
        mWorkers.emplace_back(
            std::make_unique<CWorker>(*this, mCondition, mConditionMutex));
        mWorkers.back()->Start();
    }
}

CWorkerHandler::~CWorkerHandler() {
    for (auto& worker : mWorkers) {
        worker->Stop();
    }
    {
        std::lock_guard<std::mutex> lock(mConditionMutex);
        mCondition.notify_all();
    }
    for (auto& worker : mWorkers) {
        worker->Join();
    }
}

CWorker& CWorkerHandler::GetLeastLoadedWorker() {
    CWorker* leastLoadedWorker = nullptr;
    std::size_t minTasks = std::numeric_limits<std::size_t>::max();

    for (auto& worker : mWorkers) {
        std::size_t taskCount = worker->QueuedTasksCount();
        if (taskCount == 0) {
            return *worker;
        }

        if (taskCount < minTasks) {
            minTasks = taskCount;
            leastLoadedWorker = worker.get();
        }
    }
    return *leastLoadedWorker;
}

std::optional<std::function<void()>>
CWorkerHandler::Steal(CWorker* requestingWorker) {
    for (const auto& worker : mWorkers) {
        if (worker.get() != requestingWorker) {
            auto stolenTask = worker->Steal();
            if (stolenTask.has_value()) {
                return stolenTask;
            }
        }
    }
    return std::nullopt;
}
bool CWorkerHandler::HasJobToSteal() const {
    for (const auto& worker : mWorkers) {
        if (worker->QueuedTasksCount() > 1) {
            return true;
        }
    }
    return false;
}

} // namespace Thread
