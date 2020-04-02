#include "Logging.h"
#include "AsyncLogging.h"
#include "CurrentThread.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging* AsyncLogger_;

void once_init() {
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char* msg, int len) {
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

std::string Logger::logFileName_ = "./scc_WebServer.log";

void Logger::setLogFileName(std::string fileName) {
    logFileName_ = fileName;
}

std::string Logger::getLogFileName() {
    return logFileName_;
}

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "TRACE ",
    "DEBUG ",
    "INFO ",
    "WARN ",
    "ERROR ",
    "FATAL "
};

Logger::Impl::Impl(LogLevel level, const char* fileName, int line) : stream_(),
level_(level), line_(line), basename_(fileName) {
    stream_ << LogLevelName[level_] << " ";
    CurrentThread::tid();
    stream_ << CurrentThread::tidString() << " ";
    formatTime();
}

void Logger::Impl::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char* filename, int line) : impl_(Logger::INFO, filename, line) 
{}

Logger::Logger(const char* filename, int line, LogLevel level) : impl_(level, filename, line)
{}

Logger::~Logger() {
    impl_.stream_ << "--" << impl_.basename_ << ":" << impl_.line_ << "\n";
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}

