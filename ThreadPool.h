#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "base/Mutex.h"
#include "base/Condition.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
#include "Callbacks.h"

#include <atomic>
#include <vector>
#include <deque>

class ThreadPool : noncopyable {
public:
    explicit ThreadPool(size_t numThread, size_t maxQueueSize = 65536,
                        const ThreadInitCallback& cb = nullptr);
    ~ThreadPool();

    void runTask(const Task& task);
    void runTask(Task&& task);
    void stop();
    size_t numThreads() const { return threads_.size(); }

private:
    void runInThread(size_t index);
    Task take();

    typedef std::unique_ptr<Thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadList;

    ThreadList threads_;
    MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    std::deque<Task> taskQueue_;
    const size_t maxQueueSize_;
    std::atomic_bool running_;
    ThreadInitCallback threadInitCallback_;
};

#endif