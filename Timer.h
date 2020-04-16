#ifndef TIMER_H
#define TIMER_H

#include <assert.h>
#include "Callbacks.h"
#include <functional>
#include "Channel.h"
#include "base/Timestamp.h"
#include "base/noncopyable.h"

class Timer : noncopyable {
public:
    Timer(TimerCallback callback, Timerstamp when, Nanosecond interval)
            : callback_(std::move(callback)), 
              when_(when)
              interval_(interval),
              repeat_(interval_ > Nanosecond::zero()),
              canceled_(false) {

    }

    void run() {
        if(callback_)
            callback_();
    }

    bool repeat() const {
        return repeat_;
    }

    bool expired(Timestamp now) const {
        return (now >= when_);
    }

    Timestamp when() const {
        return when_;
    }

    void restart() {
        assert(repeat_);
        when_ += interval_;
    }

    void cancel() {
        assert(!canceled_);
        canceled_ = true;
    }

    bool canceled() const {
        return canceled_;
    }

private:
    TimerCallback callback_;
    Timestamp when_;
    const Nanosecond interval_;
    bool repeat_;
    bool canceled_;
};

#endif