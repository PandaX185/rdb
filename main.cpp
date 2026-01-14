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

    net::TCPServer server(6666, [&dispatcher](const std::string &request) -> std::string
                          {
        std::istringstream iss(request);
        std::string command_name;
        iss >> command_name;

        Command command;
        command.name = toupper(command_name);
        std::string arg;
        while (iss >> arg)
        {
            command.args.push_back(arg);
        }

        Response response = dispatcher.dispatch(command);
        return response.to_resp(); });

    server.start();
    return 0;
}
