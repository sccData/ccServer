#include "EventLoop.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <signal.h>

__thread EventLoop* t_loopInThisThread = 0;

class IgnoreSigPipe {
public:
    IgnoreSigPipe() {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe ignore;

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_= " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

EventLoop::EventLoop() : looping_(false), threadId_(CurrentThread::tid()), 
                         quit_(false), doingPendingTasks_(false), 
                         poller_(this), wakeup(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
                         wakeupChannel_(this, wakeupFd_), timerQueue_(this) {
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if(t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }

    if(wakeupFd_ == -1)
        LOG_FATAL << "EventLoop::eventfd()";
    wakeupChannel_.setReadCallback([this]() { handleRead(); });
    wakeupChannel_.enableRead();
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    LOG_TRACE << "EventLoop " << this << " polling";
    looping_ = true;
    quit_ = false;

    while(!quit_) {
        activeChannels_.clear();
        poller_.poll(activeChannels_);
        for(auto channel : activeChannels_)
            channel->handleEvent();
        doPendingTasks();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

EventLoop::~EventLoop() {
    assert(!looping_);
    wakeupChannel_.diableAll();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::quit() {
    assert(!quit_);
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}

void EventLoop::runInLoop(const Task& task) {
    if(isInLoopThread())
        task();
    else
        queueInLoop(task);
}

void EventLoop::runInLoop(Task&& task) {
    if(isInLoopThread())
        task();
    else
        queueInLoop(std::move(task));
}

void EventLoop::queueInLoop(const Task& task) {
    {
        MutexLockGuard lock(mutex_);
        pendingTasks_.push_back(task);
    }

    if(!isInLoopThread() || doingPendingTasks_)
        wakeup();
}

void EventLoop::queueInLoop(Task&& task) {
    {
        MutexLockGuard lock(mutex_);
        pendingTasks_.push_back(std::move(task));
    }

    if(!isInLoopThread() || doingPendingTasks_)
        wakeup();
}

Timer* EventLoop::runAt(Timestamp when, TimerCallback callback) {
    return timerQueue_.addTimer(std::move(callback), when, Millisecond::zero());
}

Timer* EventLoop::runAfter(Nanosecond interval, TimerCallback callback) {
    return runAt(clock::now() + interval, std::move(callback));
}

Timer* EventLoop::runEvery(Nanosecond interval, TimerCallback callback) {
    return timerQueue_.addTimer(std::move(callback), clock::now() + interval, interval);
}

void EventLoop::cancelTimer(Timer* timer) {
    timerQueue_.cancelTimer(timer);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
        LOG_ERROR << "EventLoop::wakeup should ::write() " << sizeof(one) << " bytes";
}

void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread();
    poller_.updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assertInLoopThread();
    channel->diableAll();
}

void EventLoop::doPendingTasks() {
    assertInLoopThread();
    std::vector<Task> tasks;
    {
        MutexLockGuard lock(mutex_);
        tasks.swap(pendingTasks_);
    }

    doingPendingTasks_ = true;
    for(Task& task : tasks) 
        task();
    doingPendingTasks_ = false;
}

void EventLoop::handleRead() {
    uint64_t one;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
        LOG_ERROR << "EventLoop::handleRead() should ::read() " << sizeof(one) << " bytes";
}
