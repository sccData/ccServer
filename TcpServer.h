#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpServerSingle.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "base/noncopyable.h"
#include "base/Thread.h"
#include "base/Mutex.h"
#include "base/Condition.h"

#include <vector>
#include <atomic>

class EventLoopThread;
class TcpServerSingle;
class EventLoop;
class InetAddress;

class TcpServer : noncopyable {
public:
    TcpServer(EventLoop* loop, const InetAddress& local);
    ~TcpServer();
    void setNumThread(size_t n);
    void start();

    void setThreadInitCallback(const ThreadInitCallback& cb) {
        threadInitCallback_ = cb;
    }

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

private:
    void startInLoop();
    void runInThread(size_t index);

    typedef std::unique_ptr<Thread> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadPtrList;
    typedef std::unique_ptr<TcpServerSingle> TcpServerSinglePtr;
    typedef std::vector<EventLoop*> EventLoopList;

    EventLoop* baseLoop_;
    TcpServerSinglePtr baseServer_;
    ThreadPtrList threads_;
    EventLoopList eventLoops_;
    size_t numThreads_;
    std::atomic_bool started_;
    InetAddress local_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback threadInitCallback_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
};

#endif