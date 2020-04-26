#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "base/noncopyable.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "Channel.h"
#include "InetAddress.h"
#include "anyone.h"

class EventLoop;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, int sockfd, const InetAddress& local, const InetAddress& peer);
    ~TcpConnection();

    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t mark) {
        highWaterMarkCallback_ cb;
        highWaterMark_ = mark;
    }

    void setCloseCallBack(const EventCallback& cb) {
        closeCallback_ = cb;
    }

    void connectionEstablished();

    bool connected() const;
    bool disconnected() const;

    const InetAddress& local() const {
        return local_;
    }

    const InetAddress& peer() const {
        return peer_;
    }

    std::string name() const {
        return peer_.toIpPort() + "->" + local_.toIpPort();
    }

    void setContext(const std::any)

private:
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const char* data, size_t len);
    void sendInLoop(const std::string& message);
    void shutdownInLoop();
    void forceCloseInLoop();

    int stateAtomicGetAndSet(int newState);

    EventLoop* loop_;
    const int sockfd_;
    Channel channel_;
    int state_;
    InetAddress local_;
    InetAddress peer_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    size_t highWaterMark_;
    any context_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    EventCallback closeCallback_;
};

#endif