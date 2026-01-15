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

Connect with a Redis client (e.g., redis-cli) or send RESP-formatted commands via telnet/netcat:

Example RESP command for SET: `*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n`

Responses are RESP-compliant.

## Architecture

- **TCP Server**: Uses epoll for event-driven I/O
- **Store**: Simple key-value store
- **Dispatcher**: Command parsing and execution

## License

MIT