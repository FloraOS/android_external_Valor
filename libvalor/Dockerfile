FROM alpine:latest AS builder
LABEL authors="Anna-Sophie Zaitsewa"

# Prepare enviroment
WORKDIR /opt
RUN apk update
RUN apk add cmake make gcc clang

# Copy sources
RUN mkdir libvalor
WORKDIR /opt/libvalor
COPY src src
COPY include include
COPY tests tests
COPY CMakeLists.txt CMakeLists.txt

# Prepare build
RUN mkdir build

# Run buildlibvalor
WORKDIR /opt/libvalor/build
RUN cmake ..
RUN make

# Create image with library
FROM alpine:latest
COPY --from=builder /opt/libvalor/build/libvalor.a /lib/
COPY include /usr/include/valor