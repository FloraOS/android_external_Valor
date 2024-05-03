FROM ghcr.io/0x50f13/valor_libvalor:main AS builder

RUN apk update
RUN apk add cmake make gcc clang

WORKDIR /opt
RUN mkdir dbgen

WORKDIR /opt/dbgen
COPY CMakeLists.txt CMakeLists.txt
COPY src src
RUN mkdir build
WORKDIR /opt/dbgen/build

RUN cmake .. -DEO_SOURCE_DIR:PATH=/usr/include
RUN make

FROM alpine:latest
COPY --from=builder /opt/dbgen/build/dbgen /usr/bin/dbgen