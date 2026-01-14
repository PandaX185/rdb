#include <string>
#include <vector>
#include <sstream>
namespace core
{
    enum class ResponseStatus
    {
        OK,
        ERROR,
        STRING,
        NIL,
        ARRAY,
        INTEGER
    };

    class Response
    {
    public:
        ResponseStatus status;
        std::string message;
        std::vector<std::string> array_data;
        long long int_value;

        Response(ResponseStatus status, const std::string &message = "", const std::vector<std::string> &array = {}, long long int_val = 0)
            : status(status), message(message), array_data(array), int_value(int_val) {}

        static Response Ok()
        {
            return Response(ResponseStatus::OK, "");
        }

        static Response Error(const std::string &msg)
        {
            return Response(ResponseStatus::ERROR, msg);
        }

        static Response String(const std::string &msg)
        {
            return Response(ResponseStatus::STRING, msg);
        }

        static Response Nil()
        {
            return Response(ResponseStatus::NIL);
        }

        static Response Array(const std::vector<std::string> &arr)
        {
            return Response(ResponseStatus::ARRAY, "", arr);
        }

        static Response Integer(long long val)
        {
            return Response(ResponseStatus::INTEGER, "", {}, val);
        }

        std::string to_resp() const
        {
            std::ostringstream oss;
            switch (status)
            {
            case ResponseStatus::OK:
                oss << "+OK\r\n";
                break;
            case ResponseStatus::ERROR:
                oss << "-ERR " << message << "\r\n";
                break;
            case ResponseStatus::STRING:
                oss << "$" << message.size() << "\r\n"
                    << message << "\r\n";
                break;
            case ResponseStatus::NIL:
                oss << "$-1\r\n";
                break;
            case ResponseStatus::ARRAY:
                oss << "*" << array_data.size() << "\r\n";
                for (const auto &item : array_data)
                {
                    oss << "$" << item.size() << "\r\n"
                        << item << "\r\n";
                }
                break;
            case ResponseStatus::INTEGER:
                oss << ":" << int_value << "\r\n";
                break;
            }
            return oss.str();
        }
    };
}
