#include "engine/utils/ThreadPool.h"

namespace Utils {
CThreadPool::CThreadPool(std::size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        mThreads.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mTasksMutex);

                    mCondition.wait(lock, [this] {
                        return !mTasks.empty() || mShouldStop;
                    });

                    if (mShouldStop && mTasks.empty()) {
                        return;
                    }

                    task = std::move(mTasks.front());
                    mTasks.pop();
                }

                task();
                if (--mPendingTasks == 0 && mTasks.empty()) {
                    mCompletionCondition.notify_all();
                }
            }
        });
    }
}
CThreadPool::~CThreadPool() {
    mShouldStop = true;
    mCondition.notify_all();
    for (auto& thread : mThreads) {
        thread.join();
    }
}

void CThreadPool::EnqueueJob(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mTasksMutex);
        mTasks.emplace(std::move(task));
    }
    mPendingTasks++;
    mCondition.notify_one();
}

void CThreadPool::WaitForCompletion() {
    std::unique_lock<std::mutex> lock(mTasksMutex);
    mCompletionCondition.wait(lock, [this] { return mTasks.empty(); });
}
} // namespace Utils
