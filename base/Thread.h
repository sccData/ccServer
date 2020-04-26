#ifndef BASE_THREAD_H
#define BASE_THREAD_H
#include "CountDownLatch.h"
#include <functional>
#include <memory>
#include <pthread.h>
#include <string>
#include <atomic>

class Thread : noncopyable {
public:
    typedef std::function<void()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string& name = std::string());
    ~Thread();

    void start();
    int join();

    bool started()            const { return started_; }
    pid_t tid()               const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated()         { return numCreated_.load(); }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    static std::atomic_int numCreated_;
};

#endif