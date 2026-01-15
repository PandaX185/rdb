#include <iostream>
#include <sstream>
#include "core/dispatcher.hpp"
#include "net/tcp_server.hpp"

using namespace core;

std::string toupper(const std::string &str)
{
    std::string result = str;
    for (char &c : result)
    {
        c = std::toupper(c);
    }
    return result;
}

int main(int argc, char *argv[])
{
    Store store;
    CommandDispatcher dispatcher(store);

    net::TCPServer server(6666, [&dispatcher](const Command &command) -> Response
                          { return dispatcher.dispatch(command); });

    server.start();
    return 0;
}
