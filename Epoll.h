#ifndef EPOLL_H
#define EPOLL_H
#include "base/noncopyable.h"
#include <vector>

class EventLoop;
class Channel;

class Epoll : noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;
    explicit Epoll(EventLoop* loop);
    ~Epoll();

    void poll(ChannelList& activeChannels);
    void updateChannel(Channel* channel);

private:
    void updateChannel(int op, Channel* channel);
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    int epollfd_;
};
#endif