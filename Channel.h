#ifndef CHANNEL_H
#define CHANNEL_H

#include "base/noncopyable.h"
#include <functional>
#include <sys/epoll.h>
#include <memory>

class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();
    void setReadCallback(const EventCallback& cb) {
        readCallback_ = cb;
    }
    void setWriteCallback(const EventCallback& cb) {
        writeCallback_ = cb;
    }
    void setCloseCallback(const EventCallback& cb) {
        closeCallback_ = cb;
    }
    void setErrorCallback(const EventCallback& cb) {
        errorCallback_ = cb;
    }

    bool polling;
    int  fd()                  const { return fd_; }
    int  events()              const { return events_; }
    void set_revents(int revt)       { revents_ = revt; }
    bool isNoneEvent()         const { return events_ == 0; }

    void tie(const std::shared_ptr<void>& obj);

    void enableRead() { 
        events_ |= (EPOLLIN | EPOLLPRI);
        update();
    }
    void enableWrite() {
        events_ |= EPOLLOUT;
        update();
    }
    void disableRead() {
        events_ &= ~EPOLLIN;
        update();
    }
    void disableWrite() {
        events_ &= ~EPOLLOUT;
        update();
    }
    void diableAll() {
        events_ = 0;
        update();
    }

    bool isReading() const { return events_ & EPOLLIN; }
    bool isWriting() const { return events_ & EPOLLOUT; }

private:
    void update();
    void remove();

    void handleEventWithGuard();

    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;

    std::weak_ptr<void> tie_;
    bool tied_;

    bool handlingEvents_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};
#endif