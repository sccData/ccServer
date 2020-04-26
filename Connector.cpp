#include "InetAddress.h"
#include "EventLoop.h"
#include "base/Logging.h"
#include "Connector.h"

#include <unistd.h>
#include <sys/socket.h>
#include <assert.h>

int createSocket() {
    int ret = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if(ret == -1)
        LOG_FATAL << "Connector::socket()";
    return ret;
}

Connector::Connector(EventLoop* loop, const InetAddress& peer) : loop_(loop), peer_(peer),
                                                                 sockfd_(createSocket()), connected_(false),
                                                                 started_(false), channel_(loop, sockfd_) {
    channel_.setWriteCallback([this]() { handleWrite(); });
}

Connector::~Connector() {
    if(!connected_)
        ::close(sockfd_);
}

void Connector::start() {
    loop_->assertInLoopThread();
    assert(!started_);

    int ret = ::connect(sockfd_, peer_.getSockaddr(), peer_.getSocklen());
    if(ret == -1) {
        if(errno != EINPROGRESS)
            handleWrite();
        else
            channel_.enableWrite();
    } else
        handleWrite();
}

void Connector::handleWrite() {
    loop_->assertInLoopThread();
    assert(started_);

    loop_->removeChannel(&channel_);

    int err;
    socklen_t len = sizeof(err);
    int ret = ::getsockopt(sockfd_, SOL_SOCKET, SO_ERROR, &err, &len);
    if(ret == 0)
        errno = err;
    if(errno != 0) {
        LOG_ERROR << "Connector::connect()";
        if(errorCallback_)
            errorCallback_();
    } else if(newConnectionCallback_) {
        struct sockaddr_in addr;
        len = sizeof(addr);
        void* any = &addr;
        ret = ::getsockname(sockfd_, static_cast<sockaddr*>(any), &len);
        if(ret == -1)
            LOG_ERROR << "Connection::getsockname()";
        InetAddress local;
        local.setAddress(addr);

        connected_ = true;
        newConnectionCallback_(sockfd_, local, peer_);
    }
}