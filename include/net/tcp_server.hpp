#pragma once
#include <functional>
#include <string>
#include "core/thread_pool.hpp"

namespace net
{
    using RequestHandler = std::function<std::string(const std::string &)>;

    class TCPServer
    {
    private:
        int port;
        RequestHandler handler;
        pool::ThreadPool &thread_pool;

    public:
        TCPServer(pool::ThreadPool &thread_pool, int port, RequestHandler handler) : port(port), handler(handler), thread_pool(thread_pool) {}
        void start();
    };
}
