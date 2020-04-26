#include "base/Logging.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "TcpServer.h"

TcpServer::TcpServer(EventLoop* loop, const InetAddress& local) : baseLoop_(loop),
                                                                  numThreads_(1),
                                                                  started_(false),
                                                                  local_(local),
                                                                  mutex_(),
                                                                  cond_(mutex_),
                                                                  threadInitCallback_(defaultThreadInitCallback),
                                                                  connectionCallback_(defaultConnectionCallback),
                                                                  messageCallback_(defaultMessageCallback) {
    LOG_INFO << "create TcpServer() %s" << local.toIpPort().c_str;
}

TcpServer::~TcpServer() {
    for(auto& loop : eventLoops_)
        if(loop != nullptr)
            loop->quit();
    
    for(auto& thread : threads_)
        thread->join();
    LOG_TRACE << "~TcpServer()";
}

void TcpServer::setNumThread(size_t n) {
    baseLoop_->assertInLoopThread();
    assert(n > 0);
    assert(!started_);
    numThreads_ = n;
    eventLoops_.resize(n);
}

void TcpServer::start() {
    if(started_.exchange(true))
        return;
    baseLoop_->runInLoop([=](){ startInLoop(); })
}

void TcpServer::startInLoop() {
    LOG_INFO << "TcpServer::start() " << local_.toIpPort().c_str() <<
             " with " << numThreads_ << " eventLoop thread(s)";
    baseServer_ = std::unique_ptr<TcpServerSingle>(baseLoop_, local_);
    baseServer_->setConnectionCallback(connectionCallback_);
    baseServer_->setMessageCallback(messageCallback_);
    baseServer_->setWriteCompleteCallback(writeCompleteCallback_);
    threadInitCallback_(0);
    baseServer_->start();

    for(size_t i = 1; i < numThreads_; ++i) {
        auto thread = new Thread(std::bind(&TcpServer::runInThread, this, i));
        {
            MutexLockGuard lock(mutex_);
            while(eventLoops_[i] == nullptr)
                cond_.wait();
        }

        threads_.emplace_back(Thread);
    }
}

void TcpServer::runInThread(size_t index) {
    EventLoop loop;
    TcpServerSingle server(&loop, local_);

    server.setConnectionCallback(connectionCallback_);
    server.setMessageCallback(messageCallback_);
    server.setWriteCompleteCallback(writeCompleteCallback_);

    {
        MutexLockGuard lock(mutex_);
        eventLoops_[index] = &loop;
        cond_.notify();
    }

    threadInitCallback_(index);
    server.start();
    loop.loop();
    eventLoops_[index] = nullptr;
}