#include "core/dispatcher.hpp"

namespace core
{
    CommandDispatcher::CommandDispatcher(Store &store) : store_(store)
    {
        handlers_["SET"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 2)
            {
                return Response::Error("SET command requires 2 arguments");
            }
            const std::string &key = command.args[0];
            const std::string &value = command.args[1];
            if (store_.set(key, value))
            {
                return Response::Ok();
            }
            return Response::Error("Failed to set value");
        };

        handlers_["GET"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("GET command requires 1 argument");
            }
            const std::string &key = command.args[0];
            auto result = store_.get(key);
            if (result)
            {
                return Response::String(*result);
            }
            return Response(ResponseStatus::NIL, "");
        };

        handlers_["DEL"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("DEL command requires 1 argument");
            }
            const std::string &key = command.args[0];
            if (store_.remove(key))
            {
                return Response::Ok();
            }
            return Response::Error("Key not found");
        };
    }

    Response CommandDispatcher::dispatch(const Command &command)
    {
        auto it = handlers_.find(command.name);
        if (it != handlers_.end())
        {
            return it->second(command);
        }
        return Response::Error("Unknown command: " + command.name);
    }
}
