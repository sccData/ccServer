#ifndef BASE_CONDITION_H
#define BASE_CONDITION_H
#include "noncopyable.h"
#include "Mutex.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>

class Condition : noncopyable {
public:
    explicit Condition(MutexLock& mutex) : mutex_(mutex) {
        pthread_cond_init(&pcond_, NULL);
    }

    void wait() {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    void notify() {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll() {
        pthread_cond_broadcast(&pcond_);
    }

    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
    }

    ~Condition() {
        pthread_cond_destroy(&pcond_);
    }

private:
    pthread_cond_t pcond_;
    MutexLock& mutex_;
};

#endif