#ifndef BASE_COUNTDOWNLATCH_H
#define BASE_COUNTDOWNLATCH_H
#include "noncopyable.h"
#include "Condition.h"

class CountDownLatch : noncopyable {
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;
    
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

#endif