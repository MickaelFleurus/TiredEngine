#include "engine/thread/Group.h"

#include "engine/thread/WorkerHandler.h"

namespace Thread {
CGroup::CGroup(std::size_t totalTasks, CWorkerHandler& workerHandler)
    : mTaskLeft(totalTasks), mWorkerHandler(workerHandler) {
}

void CGroup::WaitForCompletion() {
    while (auto work = mWorkerHandler.Steal(nullptr)) {
        work.value()();
    }
    if (mTaskLeft > 0) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock, [this] -> bool { return mTaskLeft == 0; });
    }
}

void CGroup::OnTaskDone() {

    std::lock_guard<std::mutex> lock(mMutex);
    --mTaskLeft;
    mCondition.notify_all();
}
} // namespace Thread
