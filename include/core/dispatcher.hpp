#pragma once
#include "command.hpp"
#include "response.hpp"
#include <unordered_map>
#include <functional>
#include "store.hpp"

namespace core
{
    class CommandDispatcher
    {
    private:
        using CommandHandler = std::function<Response(const Command &)>;
        std::unordered_map<std::string, CommandHandler> handlers_;
        Store &store_;

    public:
        explicit CommandDispatcher(Store &store);
        Response dispatch(const Command &command);
    };
}
