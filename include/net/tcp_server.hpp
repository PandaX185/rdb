#pragma once
#include <functional>
#include <string>
#include "core/command.hpp"
#include "core/response.hpp"

namespace net
{
    using RequestHandler = std::function<core::Response(const core::Command &)>;

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
