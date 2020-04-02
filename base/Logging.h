#ifndef BASE_LOGGING_H
#define BASE_LOGGING_H
#include <string>
#include "LogStream.h"
#include <pthread.h>

class AsyncLogging;

class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    Logger(const char* filename, int line);
    Logger(const char* filename, int line, LogLevel level);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static void setLogFileName(std::string fileName);
    static std::string getLogFileName();
private:
    class Impl {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, const char* file_name, int line);
        void formatTime();

        LogStream stream_;
        LogLevel level_;
        int line_;
        std::string basename_;
    };

    Impl impl_;
    static std::string logFileName_;
};

#define LOG_TRACE Logger(__FILE__, __LINE__, Logger::TRACE).stream()
#define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG).stream()
#define LOG_INFO Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#endif