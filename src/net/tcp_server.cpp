#include "net/tcp_server.hpp"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <string>

namespace net
{
    struct ClientState
    {
        std::string read_buffer;
        std::string write_buffer;
    };

    void set_nonblock(int fd)
    {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    void TCPServer::start()
    {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        set_nonblock(server_fd);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        bind(server_fd, (sockaddr *)&addr, sizeof(addr));
        listen(server_fd, 16);

        std::cout << "Server started on port " << port << std::endl;

        int epfd = epoll_create1(0);
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = server_fd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

        std::map<int, ClientState> clients;
        const int MAX_EVENTS = 64;
        struct epoll_event events[MAX_EVENTS];

        while (true)
        {
            int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
            for (int i = 0; i < n; ++i)
            {
                int fd = events[i].data.fd;
                if (fd == server_fd)
                {
                    int client_fd;
                    while ((client_fd = accept(server_fd, nullptr, nullptr)) >= 0)
                    {
                        set_nonblock(client_fd);
                        ev.events = EPOLLIN;
                        ev.data.fd = client_fd;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                        clients[client_fd] = ClientState{};
                    }
                }
                else
                {
                    auto &state = clients[fd];
                    if (events[i].events & EPOLLIN)
                    {
                        char buf[4096];
                        ssize_t nread = read(fd, buf, sizeof(buf));
                        if (nread > 0)
                        {
                            state.read_buffer.append(buf, nread);
                            size_t pos;
                            while ((pos = state.read_buffer.find('\n')) != std::string::npos)
                            {
                                std::string line = state.read_buffer.substr(0, pos);
                                state.read_buffer.erase(0, pos + 1);
                                if (!line.empty() && line.back() == '\r')
                                    line.pop_back();
                                std::string response = handler(line);
                                state.write_buffer += response;
                            }
                            if (!state.write_buffer.empty())
                            {
                                ev.events = EPOLLIN | EPOLLOUT;
                                ev.data.fd = fd;
                                epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
                            }
                        }
                        else if (nread == 0)
                        {
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                            close(fd);
                            clients.erase(fd);
                        }
                        else
                        {
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                            close(fd);
                            clients.erase(fd);
                        }
                    }
                    if (events[i].events & EPOLLOUT)
                    {
                        ssize_t nwrite = write(fd, state.write_buffer.data(), state.write_buffer.size());
                        if (nwrite > 0)
                        {
                            state.write_buffer.erase(0, nwrite);
                            if (state.write_buffer.empty())
                            {
                                ev.events = EPOLLIN;
                                ev.data.fd = fd;
                                epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
                            }
                        }
                        else
                        {
                            epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                            close(fd);
                            clients.erase(fd);
                        }
                    }
                }
            }
        }
    }
}
