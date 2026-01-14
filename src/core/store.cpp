#include "core/store.hpp"
#include <unordered_map>
#include <vector>
#include "core/value.hpp"
#include <shared_mutex>
#include <algorithm>
#include <mutex>

namespace core
{

    class Shard
    {
    public:
        std::unordered_map<std::string, Value> data;
        mutable std::shared_mutex mtx;
    };

    class StoreImpl
    {
    public:
        std::vector<Shard> shards;
        StoreImpl(size_t num_shards = 16) : shards(num_shards) {}
    };

    static StoreImpl impl;

    std::optional<std::string> Store::get(const std::string &key) const
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        const Shard &shard = impl.shards[shard_index];
        std::shared_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::STRING)
        {
            return std::get<std::string>(it->second.data);
        }
        return std::nullopt;
    }

    bool Store::set(const std::string &key, const std::string &value)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        shard.data.insert_or_assign(key, Value(value));
        return true;
    }

    bool Store::remove(const std::string &key)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        return shard.data.erase(key) > 0;
    }

    bool Store::lpush(const std::string &key, const std::string &value)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it == shard.data.end())
        {
            shard.data.emplace(key, Value(std::deque<std::string>{value}));
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it == shard.data.end())
        {
            shard.data.emplace(key, Value(std::deque<std::string>{value}));
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::LIST)
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::LIST)
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::shared_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::LIST)
        {
            auto &list = std::get<std::deque<std::string>>(it->second.data);
            return list.size();
        }
        return std::nullopt;
    }

    std::optional<std::deque<std::string>> Store::lrange(const std::string &key, int start, int end)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::shared_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::LIST)
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it == shard.data.end())
        {
            shard.data.emplace(key, Value(std::unordered_set<std::string>{value}));
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
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::SET)
        {
            auto &set = std::get<std::unordered_set<std::string>>(it->second.data);
            return set.erase(value) > 0;
        }
        return false;
    }

    std::optional<std::unordered_set<std::string>> Store::sismember(const std::string &key)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::unique_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::SET)
        {
            return std::get<std::unordered_set<std::string>>(it->second.data);
        }
        return std::nullopt;
    }

    std::optional<size_t> Store::scard(const std::string &key)
    {
        auto shard_index = std::hash<std::string>{}(key) % impl.shards.size();
        Shard &shard = impl.shards[shard_index];
        std::shared_lock lock(shard.mtx);
        auto it = shard.data.find(key);
        if (it != shard.data.end() && it->second.type == ValueType::SET)
        {
            auto &set = std::get<std::unordered_set<std::string>>(it->second.data);
            return set.size();
        }
        return std::nullopt;
    }

    std::optional<std::unordered_set<std::string>> Store::sinter(const std::string &key1, const std::string &key2)
    {
        auto index1 = std::hash<std::string>{}(key1) % impl.shards.size();
        auto index2 = std::hash<std::string>{}(key2) % impl.shards.size();
        if (index1 == index2)
        {
            Shard &shard = impl.shards[index1];
            std::shared_lock lock(shard.mtx);
            auto it1 = shard.data.find(key1);
            auto it2 = shard.data.find(key2);
            if (it1 != shard.data.end() && it1->second.type == ValueType::SET &&
                it2 != shard.data.end() && it2->second.type == ValueType::SET)
            {
                auto &set1 = std::get<std::unordered_set<std::string>>(it1->second.data);
                auto &set2 = std::get<std::unordered_set<std::string>>(it2->second.data);
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
        else
        {
            size_t min_index = std::min(index1, index2);
            size_t max_index = std::max(index1, index2);
            Shard &shard_min = impl.shards[min_index];
            Shard &shard_max = impl.shards[max_index];
            std::shared_lock lock_min(shard_min.mtx);
            std::shared_lock lock_max(shard_max.mtx);
            const std::string &key_min = (min_index == index1) ? key1 : key2;
            const std::string &key_max = (min_index == index1) ? key2 : key1;
            auto it_min = shard_min.data.find(key_min);
            auto it_max = shard_max.data.find(key_max);
            if (it_min != shard_min.data.end() && it_min->second.type == ValueType::SET &&
                it_max != shard_max.data.end() && it_max->second.type == ValueType::SET)
            {
                auto &set_min = std::get<std::unordered_set<std::string>>(it_min->second.data);
                auto &set_max = std::get<std::unordered_set<std::string>>(it_max->second.data);
                std::unordered_set<std::string> result;
                for (const auto &item : set_min)
                {
                    if (set_max.find(item) != set_max.end())
                    {
                        result.insert(item);
                    }
                }
                return result;
            }
            return std::nullopt;
        }
    }
}
