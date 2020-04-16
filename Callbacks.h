#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <memory>
#include <functional>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

class Buffer;
class TcpConnection;
class InetAddress;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer&)> MessageCallback;

typedef std::function<void()> ErrorCallback;
typedef std::function<void(int sockfd, const InetAddress& local, const InetAddress& peer)> NewConnectionCallback;

typedef std::function<void()> Task;
typedef std::function<void()> TimerCallback;
#endif