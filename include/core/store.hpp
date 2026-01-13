#pragma once
#include <optional>
#include <string>
#include <deque>
#include <unordered_set>

namespace core
{
    class Store
    {
    public:
        // String operations
        bool set(const std::string &key, const std::string &value);
        std::optional<std::string> get(const std::string &key) const;
        bool remove(const std::string &key);

        // List operations
        bool lpush(const std::string &key, const std::string &value);
        bool rpush(const std::string &key, const std::string &value);
        std::optional<std::string> lpop(const std::string &key);
        std::optional<std::string> rpop(const std::string &key);
        std::optional<std::deque<std::string>> lrange(const std::string &key, int start, int end);
        std::optional<size_t> llen(const std::string &key);

        // Set operations
        bool sadd(const std::string &key, const std::string &value);
        bool srem(const std::string &key, const std::string &value);
        std::optional<std::unordered_set<std::string>> sismember(const std::string &key);
        std::optional<size_t> scard(const std::string &key);
        std::optional<std::unordered_set<std::string>> sinter(const std::string &key1, const std::string &key2);
    };
}
