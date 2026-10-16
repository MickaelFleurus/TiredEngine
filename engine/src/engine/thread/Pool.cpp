#include "engine/thread/Pool.h"

namespace Thread {
CPool::CPool() : mWorkerHandler(std::thread::hardware_concurrency()) {
}

CPool::~CPool() = default;

std::unique_ptr<CGroup> CPool::EnqueueJob(std::function<void()> task) {
    std::unique_ptr<CGroup> group = std::make_unique<CGroup>(1, mWorkerHandler);
    auto& worker = mWorkerHandler.GetLeastLoadedWorker();
    worker.EnqueueTask([task = std::move(task), groupPtr = group.get()]() {
        task();
        groupPtr->OnTaskDone();
    });

    return group;
}

std::unique_ptr<CGroup>
CPool::EnqueueJobs(const std::vector<std::function<void()>>& tasks) {
    std::unique_ptr<CGroup> group =
        std::make_unique<CGroup>(tasks.size(), mWorkerHandler);
    auto& worker = mWorkerHandler.GetLeastLoadedWorker();
    for (const auto& task : tasks) {
        worker.EnqueueTask([task, groupPtr = group.get()]() {
            task();
            groupPtr->OnTaskDone();
        });
    }

    return group;
}

} // namespace Thread
