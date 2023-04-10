ARG VERSION=latest

FROM ubuntu:${VERSION}

COPY . /home

WORKDIR /home

RUN apt-get update -y && \
    apt-get install -y g++ && \
    apt-get install -y clang && \
    apt-get install -y cmake && \
    apt-get install -y make && \
    apt-get -y install ninja-build && \
    mkdir -p build && cd build && \
    cmake -G Ninja .. && \
    cmake --build . && \
    cmake --install .

CMD ["bash"]
