#ifndef BASE_LOGFILE_H
#define BASE_LOGFILE_H
#include "FileUtil.h"
#include "Mutex.h"
#include "noncopyable.h"

#include <string>
#include <memory>

class LogFile : noncopyable {
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};

#endif