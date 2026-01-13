#include "core/store.hpp"
#include <unordered_map>
#include <mutex>

namespace core
{
    class StoreImpl
    {
    public:
        std::unordered_map<std::string, std::string> data;
        std::mutex mtx;
    };

    static StoreImpl impl;

    std::optional<std::string> Store::get(const std::string &key) const
    {
        std::lock_guard<std::mutex> lock(impl.mtx);
        auto it = impl.data.find(key);
        if (it != impl.data.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    bool Store::set(const std::string &key, const std::string &value)
    {
        std::lock_guard<std::mutex> lock(impl.mtx);
        impl.data[key] = std::move(value);
        return true;
    }

    bool Store::remove(const std::string &key)
    {
        std::lock_guard<std::mutex> lock(impl.mtx);
        return impl.data.erase(key) > 0;
    }
}
