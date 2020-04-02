#ifndef BASE_ASYNCLOGGING_H
#define BASE_ASYNCLOGGING_H
#include "CountDownLatch.h"
#include "Mutex.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string basename, int flushInterval = 3);
    ~AsyncLogging() {
        
    }

    void append(const char* logline, int len);

    void start() {
        // 在构造函数中latch_的值为1
        // 线程运行之后将latch_的减为0
        running_ = true;
        thread_.start();
        // 表明初始化已经完成
        latch_.wait();
    }

    void stop() {
        running_ = false;
        cond_.notify();
        thread_.join();
    }
    
private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;   // 不能进行复制，只能进行移动
    typedef std::unique_ptr<Buffer> BufferPtr;
    const int flushInterval_;                                    // 定期将缓冲区的数据写到文件中
    bool running_;
    std::string basename_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_;
};

#endif