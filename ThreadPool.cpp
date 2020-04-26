#include "base/Logging.h"
#include "ThreadPool.h"

#include <assert.h>

ThreadPool::ThreadPool(size_t numThread, size_t maxQueueSize, const ThreadInitCallback& cb) : maxQueueSize_(maxQueueSize),
                                                                                              running_(true),
                                                                                              threadInitCallback_(cb) {
    assert(maxQueueSize > 0);
    for(size_t i = 1; i <= numThread; ++i) {
        threads_.emplace_back(new Thread([this, i](){ runInThread(i); }));
    }
    LOG_TRACE << "ThreadPool() numThreads " << numThread << ", maxQueueSize " << maxQueueSize;
}

ThreadPool::~ThreadPool() {
    if(running_)
        stop();
    LOG_TRACE << "~ThreadPool()";
}

void ThreadPool::runTask(const Task& task) {
    assert(running_);

    if(threads_.empty()) {
        task();
    } else {
        MutexLockGuard lock(mutex_);
        while(taskQueue_.size() >= maxQueueSize_)
            notFull_.wait();
        taskQueue_.push_back(task);
        notEmpty_.notify();
    }
}
