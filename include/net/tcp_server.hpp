#pragma once
#include <functional>
#include <string>

namespace net
{
    using RequestHandler = std::function<std::string(const std::string &)>;

    class TCPServer
    {
    private:
        int port;
        RequestHandler handler;

    public:
        TCPServer(int port, RequestHandler handler) : port(port), handler(handler) {}
        void start();
    };
}
