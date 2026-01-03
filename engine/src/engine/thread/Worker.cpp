#include "engine/thread/Worker.h"

#include "engine/thread/WorkerHandler.h"

namespace Thread {

CWorker::CWorker(CWorkerHandler& communication,
                 std::condition_variable& condition, std::mutex& mutex)
    : mCommunication(communication)
    , mGlobalCondition(condition)
    , mConditionMutex(mutex) {
}

CWorker::~CWorker() {
    Stop();
}

void CWorker::Start() {
    if (!mThread.joinable()) {
        mThread = std::thread([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mConditionMutex);

                    mGlobalCondition.wait(lock, [this] -> bool {
                        return !mTasks.empty() || mShouldStop ||
                               mCommunication.HasJobToSteal();
                    });

                    if (mShouldStop) {
                        return;
                    }
                    if (mTasks.empty()) {
                        auto stolenTask = mCommunication.Steal(this);
                        if (stolenTask) {
                            task = std::move(*stolenTask);
                        }
                    } else {

                        task = std::move(mTasks.front());
                        mTasks.pop_front();
                    }
                }

                if (task) {
                    task();
                }
            }
        });
    }
}

std::size_t CWorker::QueuedTasksCount() {
    std::lock_guard<std::mutex> lock(mLocalMutex);
    return mTasks.size();
}

void CWorker::EnqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mLocalMutex);
        mTasks.push_back(std::move(task));
    }
    mGlobalCondition.notify_one();
}

void CWorker::Stop() {
    mShouldStop = true;
}

std::optional<std::function<void()>> CWorker::Steal() {
    std::lock_guard<std::mutex> lock(mLocalMutex);
    if (!mTasks.empty()) {
        auto task = std::move(mTasks.front());
        mTasks.pop_front();
        return task;
    }
    return std::nullopt;
}

void CWorker::Join() {
    if (!mThread.joinable()) {
        return;
    }
    mThread.join();
}

} // namespace Thread
