#include "net/tcp_server.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

namespace net
{
    void write_all(int fd, const std::string &data)
    {
        size_t total = 0;
        while (total < data.size())
        {
            ssize_t n = write(fd, data.data() + total, data.size() - total);
            if (n <= 0)
                break;
            total += n;
        }
    }

    void handle_client(int client_fd, RequestHandler handler)
    {
        char buffer[4096];
        std::string full_buffer;
        while (true)
        {
            ssize_t n = read(client_fd, buffer, sizeof(buffer));
            if (n <= 0)
                break;

            full_buffer.append(buffer, n);

            while (true)
            {
                auto pos = full_buffer.find("\n");
                if (pos == std::string::npos)
                    break;

                std::string line = full_buffer.substr(0, pos);
                full_buffer.erase(0, pos + 1);

                if (!line.empty() && line.back() == '\r')
                    line.pop_back();

                std::string response = handler(line);
                write_all(client_fd, response);
            }
        }

        close(client_fd);
    }

    void TCPServer::start()
    {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bind(server_fd, (sockaddr *)&addr, sizeof(addr));
        listen(server_fd, 16);

        std::cout << "Server started on port " << port << std::endl;

        while (true)
        {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd >= 0)
            {
                std::thread(handle_client, client_fd, handler).detach();
            }
        }
    }
}
