#include "core/store.hpp"
#include <unordered_map>
#include "core/value.hpp"

namespace core
{
    class StoreImpl
    {
    public:
        std::unordered_map<std::string, Value> data;
    };

    static StoreImpl impl;

    std::optional<std::string> Store::get(const std::string &key) const
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::STRING)
        {
            return std::get<std::string>(it->second.data);
        }
        return std::nullopt;
    }

    bool Store::set(const std::string &key, const std::string &value)
    {
        impl.data.insert_or_assign(key, Value(value));
        return true;
    }

    bool Store::remove(const std::string &key)
    {
        return impl.data.erase(key) > 0;
    }

    bool Store::lpush(const std::string &key, const std::string &value)
    {
        auto it = impl.data.find(key);
        if (it == impl.data.end())
        {
            impl.data.emplace(key, Value(std::deque<std::string>{value}));
            return true;
        }
        if (it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            list.push_front(value);
            return true;
        }
        return false;
    }

    bool Store::rpush(const std::string &key, const std::string &value)
    {
        auto it = impl.data.find(key);
        if (it == impl.data.end())
        {
            impl.data.emplace(key, Value(std::deque<std::string>{value}));
            return true;
        }
        if (it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            list.push_back(value);
            return true;
        }
        return false;
    }

    std::optional<std::string> Store::lpop(const std::string &key)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            if (!list.empty())
            {
                std::string value = list.front();
                list.pop_front();
                return value;
            }
        }
        return std::nullopt;
    }

    std::optional<std::string> Store::rpop(const std::string &key)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            if (!list.empty())
            {
                std::string value = list.back();
                list.pop_back();
                return value;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> Store::llen(const std::string &key)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            return list.size();
        }
        return std::nullopt;
    }

    std::optional<std::deque<std::string>> Store::lrange(const std::string &key, int start, int end)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            if (list.empty() || start > end)
                return std::deque<std::string>{};

            if (start < 0)
                start = 0;
            if (end >= static_cast<int>(list.size()))
                end = list.size() - 1;
            if (start > end)
                return std::deque<std::string>{};

            std::deque<std::string> result;
            for (int i = start; i <= end; ++i)
            {
                result.push_back(list[i]);
            }
            return result;
        }
        return std::nullopt;
    }

    bool Store::sadd(const std::string &key, const std::string &value)
    {
        auto it = impl.data.find(key);
        if (it == impl.data.end())
        {
            impl.data.emplace(key, Value(std::unordered_set<std::string>{value}));
            return true;
        }
        if (it->second.type == ValueType::SET)
        {
            auto &set = std::get<std::unordered_set<std::string>>(it->second.data);
            set.insert(value);
            return true;
        }
        return false;
    }

    bool Store::srem(const std::string &key, const std::string &value)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::SET)
        {
            auto &set = std::get<std::unordered_set<std::string>>(it->second.data);
            return set.erase(value) > 0;
        }
        return false;
    }

    std::optional<std::unordered_set<std::string>> Store::sismember(const std::string &key)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::SET)
        {
            return std::get<std::unordered_set<std::string>>(it->second.data);
        }
        return std::nullopt;
    }

    std::optional<size_t> Store::scard(const std::string &key)
    {
        auto it = impl.data.find(key);
        if (it != impl.data.end() && it->second.type == ValueType::SET)
        {
            auto &set = std::get<std::unordered_set<std::string>>(it->second.data);
            return set.size();
        }
        return std::nullopt;
    }

    std::optional<std::unordered_set<std::string>> Store::sinter(const std::string &key1, const std::string &key2)
    {
        auto it1 = impl.data.find(key1);
        auto it2 = impl.data.find(key2);
        if (it1 != impl.data.end() && it1->second.type == ValueType::SET &&
            it2 != impl.data.end() && it2->second.type == ValueType::SET)
        {
            const auto &set1 = std::get<std::unordered_set<std::string>>(it1->second.data);
            const auto &set2 = std::get<std::unordered_set<std::string>>(it2->second.data);
            std::unordered_set<std::string> result;
            for (const auto &item : set1)
            {
                if (set2.find(item) != set2.end())
                {
                    result.insert(item);
                }
            }
            return result;
        }
        return std::nullopt;
    }
}
