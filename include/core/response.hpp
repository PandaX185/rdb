#include <string>
#include <vector>
namespace core
{
    enum class ResponseStatus
    {
        OK,
        ERROR,
        STRING,
        NIL,
        ARRAY
    };

    class Response
    {
    public:
        ResponseStatus status;
        std::string message;
        std::vector<std::string> array_data;

        Response(ResponseStatus status, const std::string &message = "", const std::vector<std::string> &array = {})
            : status(status), message(message), array_data(array) {}

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

        static Response Array(const std::vector<std::string> &arr)
        {
            return Response(ResponseStatus::ARRAY, "", arr);
        }
    };
}
