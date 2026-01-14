#include "net/tcp_server.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

namespace net
{
    void handle_client(int client_fd, RequestHandler handler)
    {
        char buffer[4096];

        while (true)
        {
            ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
            if (n <= 0)
            {
                break;
            }
            buffer[n] = '\0';
            std::string request(buffer);

            std::string response = handler(request);

            write(client_fd, response.c_str(), response.size());
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
