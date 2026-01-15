FROM alpine:3.21 AS builder

RUN apk add --no-cache build-base cmake git

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

FROM alpine:3.21

RUN apk add --no-cache libstdc++

WORKDIR /app
COPY --from=builder /app/build/rdb .

ENV SERVER_PORT=6666
EXPOSE ${SERVER_PORT}

CMD ["sh", "-c", "./rdb $SERVER_PORT"]
