ARG VERSION=latest

FROM ubuntu:${VERSION}

COPY . /home

WORKDIR /home

RUN apt-get update -y && \
    apt-get install -y build-essential && \
    apt-get install -y clang && \
    apt-get install -y cmake && \
    apt-get install -y make && \
    apt-get -y install ninja-build && \
    mkdir -p build && cd build && \
    cmake -G Ninja .. && \
    cmake --build . && \
    cmake --install . && \
    ./01-homework-hello/01-1-simple-hello/simple-hello && \
    ./01-homework-hello/01-2-hello-libs/hello-bin-with-static-and-shared/hello-bin-dynamic && \
    ./01-homework-hello/01-2-hello-libs/hello-bin-with-static-and-shared/hello-bin-static && \
    ctest --test-dir 01-homework-hello/01-1-simple-hello && \
    ctest --test-dir 01-homework-hello/01-2-hello-libs/hello-bin-with-static-and-shared

CMD ["./bin/install/simple-hello"]
