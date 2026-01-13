#pragma once
#include <optional>
#include <string>

namespace core
{
    class Store
    {
    public:
        bool set(const std::string &key, const std::string &value);
        std::optional<std::string> get(const std::string &key) const;
        bool remove(const std::string &key);
    };
}
