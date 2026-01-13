#include <string>
namespace core
{
    enum class ResponseStatus
    {
        OK,
        ERROR,
        STRING,
        NIL
    };

    class Response
    {
    public:
        ResponseStatus status;
        std::string message;

        Response(ResponseStatus status, const std::string &message = "")
            : status(status), message(message) {}

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
    };
}
