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

    size_t port = 6666;
    if (argc > 1)
    {
        try
        {
            port = std::stoul(argv[1]);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing port number\nUsing default port" << std::endl;
        }
    }

    net::TCPServer server(port, [&dispatcher](const Command &command) -> Response
                          { return dispatcher.dispatch(command); });

    server.start();
    return 0;
}
