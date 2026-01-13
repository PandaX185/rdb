#include <variant>
#include <deque>
#include <unordered_set>
#include <string>

namespace core
{
    using ValueData = std::variant<std::string, std::deque<std::string>, std::unordered_set<std::string>>;

    enum class ValueType
    {
        STRING,
        LIST,
        SET
    };

    class Value
    {
    public:
        ValueType type;
        ValueData data;

        Value(const std::string &str) : type(ValueType::STRING), data(str) {}
        Value(const std::deque<std::string> &list) : type(ValueType::LIST), data(list) {}
        Value(const std::unordered_set<std::string> &set) : type(ValueType::SET), data(set) {}

        void operator=(const Value &other)
        {
            if (this != &other)
            {
                type = other.type;
                data = other.data;
            }
        }
    };
}
