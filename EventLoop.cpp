#include "EventLoop.h"

__thread EventLoop* t_loopInThisThread = 0;

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_= " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

EventLoop::EventLoop() : looping_(false), threadId_(CurrentThread::tid()) {
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if(t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;


    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}