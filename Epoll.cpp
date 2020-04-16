#include "Epoll.h"
#include "base/Logging.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <assert.h>

#include "EventLoop.h"

Epoll::Epoll(EventLoop* loop) : loop_(loop),
                                events_(128),
                                epollfd_(::epoll_create1(EPOLL_CLOEXEC)) {
    if(epollfd_ == -1)
        LOG_FATAL << "Epoll::epoll_create1()";
}

Epoll::~Epoll() {
    ::close(epollfd_);
}

void Epoll::poll(ChannelList& activeChannels) {
    loop_->assertInLoopThread();
    int maxEvents = static_cast<int>(events_.size());
    int nEvents = epoll_wait(epollfd_, events_.data(), maxEvents, -1);
    if(nEvents == -1) {
        if(errno != EINTR)
            LOG_ERROR << "Epoll::epoll_wait()";
    } else if(nEvents > 0) {
        for(int i = 0; i < nEvents; ++i) {
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->setRevents(events_[i].events);
            activeChannels.push_back(channel);
        }

        if(nEvents == maxEvents)
            events_.resize(2 * events_.size());
    }
}

void Epoll::updateChannel(Channel* channel) {
    loop_->assertInLoopThread();
    int op = 0;
    if(!channel->polling) {
        assert(!channel->isNodeEvents());
        op = EPOLL_CTL_ADD;
        channel->polling = true;
    } else if(!channel->isNoneEvents()) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_DEL;
        channel->polling = false;
    }

    updateChannel(op, channel);
}

void Epoll::updateChannel(int op, Channel* channel) {
    struct epoll_event ee;
    ee.events = channel->events();
    ee.date.ptr = channel;
    int ret = ::epoll_ctl(epollfd_, op, channel->fd(), &ee);
    if(ret == -1)
        LOG_ERROR << "Epoll::epoll_ctl()";
}