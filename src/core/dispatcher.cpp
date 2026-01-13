#include "core/dispatcher.hpp"

namespace core
{
    CommandDispatcher::CommandDispatcher(Store &store) : store_(store)
    {
        registerStringCommands();
        registerListCommands();
        registerSetCommands();
    }

    void CommandDispatcher::registerStringCommands()
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

    void CommandDispatcher::registerListCommands()
    {
        handlers_["LPUSH"] = [this](const Command &command) -> Response
        {
            if (command.args.size() < 2)
            {
                return Response::Error("LPUSH command requires at least 2 arguments");
            }
            const std::string &key = command.args[0];
            for (size_t i = 1; i < command.args.size(); ++i)
            {
                if (!store_.lpush(key, command.args[i]))
                {
                    return Response::Error("Failed to push to list");
                }
            }
            return Response::Ok();
        };

        handlers_["RPUSH"] = [this](const Command &command) -> Response
        {
            if (command.args.size() < 2)
            {
                return Response::Error("RPUSH command requires at least 2 arguments");
            }
            const std::string &key = command.args[0];
            for (size_t i = 1; i < command.args.size(); ++i)
            {
                if (!store_.rpush(key, command.args[i]))
                {
                    return Response::Error("Failed to push to list");
                }
            }
            return Response::Ok();
        };

        handlers_["LPOP"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("LPOP command requires 1 argument");
            }
            const std::string &key = command.args[0];
            auto result = store_.lpop(key);
            if (result)
            {
                return Response::String(*result);
            }
            return Response(ResponseStatus::NIL, "");
        };

        handlers_["RPOP"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("RPOP command requires 1 argument");
            }
            const std::string &key = command.args[0];
            auto result = store_.rpop(key);
            if (result)
            {
                return Response::String(*result);
            }
            return Response(ResponseStatus::NIL, "");
        };

        handlers_["LLEN"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("LLEN command requires 1 argument");
            }
            const std::string &key = command.args[0];
            auto result = store_.llen(key);
            if (result)
            {
                return Response::String(std::to_string(*result));
            }
            return Response::Error("Key is not a list");
        };

        handlers_["LRANGE"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 3)
            {
                return Response::Error("LRANGE command requires 3 arguments");
            }
            const std::string &key = command.args[0];
            int start = std::stoi(command.args[1]);
            int end = std::stoi(command.args[2]);
            auto result = store_.lrange(key, start, end);
            if (result)
            {
                std::vector<std::string> arr(result->begin(), result->end());
                return Response::Array(arr);
            }
            return Response::Error("Key is not a list");
        };
    }

    void CommandDispatcher::registerSetCommands()
    {
        handlers_["SADD"] = [this](const Command &command) -> Response
        {
            if (command.args.size() < 2)
            {
                return Response::Error("SADD command requires at least 2 arguments");
            }
            const std::string &key = command.args[0];
            for (size_t i = 1; i < command.args.size(); ++i)
            {
                if (!store_.sadd(key, command.args[i]))
                {
                    return Response::Error("Failed to add to set");
                }
            }
            return Response::Ok();
        };

        handlers_["SREM"] = [this](const Command &command) -> Response
        {
            if (command.args.size() < 2)
            {
                return Response::Error("SREM command requires at least 2 arguments");
            }
            const std::string &key = command.args[0];
            bool removed = false;
            for (size_t i = 1; i < command.args.size(); ++i)
            {
                if (store_.srem(key, command.args[i]))
                {
                    removed = true;
                }
            }
            return removed ? Response::Ok() : Response::Error("Member not found in set");
        };

        handlers_["SISMEMBER"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 2)
            {
                return Response::Error("SISMEMBER command requires 2 arguments");
            }
            const std::string &key = command.args[0];
            const std::string &member = command.args[1];
            auto set_opt = store_.sismember(key);
            if (set_opt)
            {
                bool is_member = set_opt->find(member) != set_opt->end();
                return Response::String(is_member ? "1" : "0");
            }
            return Response::Error("Key is not a set");
        };

        handlers_["SCARD"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 1)
            {
                return Response::Error("SCARD command requires 1 argument");
            }
            const std::string &key = command.args[0];
            auto result = store_.scard(key);
            if (result)
            {
                return Response::String(std::to_string(*result));
            }
            return Response::Error("Key is not a set");
        };

        handlers_["SINTER"] = [this](const Command &command) -> Response
        {
            if (command.args.size() != 2)
            {
                return Response::Error("SINTER command requires 2 arguments");
            }
            const std::string &key1 = command.args[0];
            const std::string &key2 = command.args[1];
            auto result = store_.sinter(key1, key2);
            if (result)
            {
                std::vector<std::string> arr(result->begin(), result->end());
                return Response::Array(arr);
            }
            return Response::Error("Keys are not sets");
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
