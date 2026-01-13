#pragma once
#include <string>
#include <vector>

namespace core
{
    class Command
    {
    public:
        std::string name;
        std::vector<std::string> args;
    };

}
