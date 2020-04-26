#include "base/Logging.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "TcpServerSingle.h"

TcpServerSingle::TcpServerSingle(EventLoop* loop, const InetAddress& local) : loop_(loop),
                                                                              acceptor_(loop, local) {
    acceptor_.setNewConnectionCallback(std::bind(&TcpServerSingle::newConnection, this, _1, _2, _3));
}

void TcpServerSingle::start() {
    acceptor_.listen();
}

void TcpServerSingle::newConnection(int connfd, const InetAddress& local, const InetAddress& peer) {
    loop_->assertInLoopThread();
    auto conn = std::make_shared<TcpConnection>(loop_, connfd, local, peer);
    connections_.insert(conn);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallBack(std::bind(&TcpServerSingle::closeConnection, this, _1));
    conn->connectEstablished();
    connectionCallback_(conn);
}

void TcpServerSingle::closeConnection(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    size_t ret = connections_.erase(conn);
    assert(ret == 1);
    (void)ret;
    connectionCallback_(conn);
}