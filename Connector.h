#ifndef CONNECTOR_H
#define CONNECTOR_H
#include "InetAddress.h"
#include "Channel.h"
#include "base/noncopyable.h"

#include <functional>

class EventLoop;
class InetAddress;

class Connector : noncopyable {
private:
    void handleWrite();

    EventLoop* loop_;
    const InetAddress peer_;
    const int sockfd_;
    bool connected_;
    bool started_;
    Channel channel_;
    NewConnectionCallback newConnectionCallback_;
    EventCallback errorCallback_;
};

#endif