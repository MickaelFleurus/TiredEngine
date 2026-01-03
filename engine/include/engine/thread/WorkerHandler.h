#pragma once

#include <vector>

#include "engine/thread/Worker.h"

namespace Thread {
class CWorkerHandler {
public:
    CWorkerHandler(std::size_t numWorkers);
    ~CWorkerHandler();

    CWorker& GetLeastLoadedWorker();

    std::optional<std::function<void()>> Steal(CWorker* requestingWorker);
    bool HasJobToSteal() const;

private:
    std::mutex mConditionMutex;

    std::condition_variable mCondition;
    std::vector<std::unique_ptr<CWorker>> mWorkers;
};
} // namespace Thread
