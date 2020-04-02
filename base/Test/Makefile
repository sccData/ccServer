#VPATH = Test:../
vpath %.cpp ../
vpath %.h ../
objects = LoggingTest.o Logging.o AsyncLogging.o LogStream.o LogFile.o \
FileUtil.o Thread.o CountDownLatch.o

LoggingTest : $(objects)
	g++ -std=c++11 -o LoggingTest $(objects) -lpthread
LoggingTest.o : LoggingTest.cpp Logging.h Thread.h
	g++ -std=c++11 -c ../LoggingTest.cpp
Logging.o : Logging.cpp Logging.h LogStream.h AsyncLogging.h CurrentThread.h
	g++ -std=c++11 -c ../Logging.cpp
AsyncLogging.o : AsyncLogging.cpp AsyncLogging.h CountDownLatch.h Mutex.h Thread.h LogStream.h noncopyable.h LogFile.h
	g++ -std=c++11 -c ../AsyncLogging.cpp
LogStream.o : noncopyable.h
LogFile.o : LogFile.cpp LogFile.h FileUtil.h Mutex.h noncopyable.h
	g++ -std=c++11 -c ../LogFile.cpp
FileUtil.o : FileUtil.cpp FileUtil.h noncopyable.h
	g++ -std=c++11 -c ../FileUtil.cpp
Thread.o : Thread.cpp Thread.h Atomic.h CountDownLatch.h CurrentThread.h
	g++ -std=c++11 -c ../Thread.cpp
CountDownLatch.o : CountDownLatch.cpp CountDownLatch.h noncopyable.h Condition.h
	g++ -std=c++11 -c ../CountDownLatch.cpp
.PHONY : clean
clean : 
	rm LoggingTest $(objects)