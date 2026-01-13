#include <iostream>
#include <sstream>
#include "core/dispatcher.hpp"

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

int main()
{
    Store store;
    CommandDispatcher dispatcher(store);

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        Command cmd;
        iss >> cmd.name;
        cmd.name = toupper(cmd.name);
        std::string arg;
        while (iss >> arg)
        {
            cmd.args.push_back(arg);
        }

        Response response = dispatcher.dispatch(cmd);
        switch (response.status)
        {
        case ResponseStatus::OK:
            std::cout << "OK" << std::endl;
            break;
        case ResponseStatus::ERROR:
            std::cout << "ERROR: " << response.message << std::endl;
            break;
        case ResponseStatus::STRING:
            std::cout << response.message << std::endl;
            break;
        case ResponseStatus::NIL:
            std::cout << "(nil)" << std::endl;
            break;
        case ResponseStatus::ARRAY:
            std::cout << "[";
            for (size_t i = 0; i < response.array_data.size(); ++i)
            {
                if (i > 0)
                    std::cout << ",";
                std::cout << response.array_data[i];
            }
            std::cout << "]" << std::endl;
            break;
        }
    }
}
