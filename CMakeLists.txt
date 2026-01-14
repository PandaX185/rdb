cmake_minimum_required(VERSION 3.10.0)
project(rdb VERSION 0.1.0 LANGUAGES C CXX)

add_executable(rdb main.cpp src/core/dispatcher.cpp src/core/store.cpp src/net/tcp_server.cpp src/core/thread_pool.cpp)

target_include_directories(rdb PRIVATE include)

