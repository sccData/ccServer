#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include "base/noncopyable.h"
#include "base/Logging.h"
#include "base/CurrentThread.h"
#include "base/Mutex.h"

#include <assert.h>
#include <functional>
#include <vector>
#include <atomic>

class EventLoop : noncopyable {
public:
    typedef std::function<void()> Task;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(const Task& task);
    void runInLoop(Task&& task);
    void queueInLoop(const Task& task);
    void queueInLoop(Task&& task);

    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread() {
        if(!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const {
        return threadId_ == CurrentThread::tid();
    }

private:
    void abortNotInLoopThread();

    void doPendingTasks();
    void handleRead();
    bool looping_;
    const pid_t threadId_;
    std::atomic_bool quit_;
    bool doingPendingTasks_;
    Epoll poller_;
    Epoll::ChannelList activeChannels_;
    const int wakeupFd_;
    Channel wakeupChannel_;
    mutable MutexLock mutex_;
    std::vector<Task> pendingTasks;

}

#endif