# RDB

A simple Redis-like in-memory data store implemented in C++.

## Features

- Supports basic Redis commands: SET, GET, DEL, LPUSH, RPUSH, LPOP, RPOP, LLEN, LRANGE, SADD, SREM, SISMEMBER, SCARD, SINTER
- Single-threaded with epoll for non-blocking I/O and concurrent clients
- RESP protocol compliant responses

## Building

Requires CMake and a C++17 compiler.

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Run the server:

```bash
./rdb
```

Connect with a Redis client or telnet:

```bash
telnet localhost 6666
SET key value
GET key
```

## Architecture

- **TCP Server**: Uses epoll for event-driven I/O
- **Store**: Simple key-value store
- **Dispatcher**: Command parsing and execution

## License

MIT