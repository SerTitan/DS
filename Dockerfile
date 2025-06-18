FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    g++ \
    cmake \
    make \
    gnuplot \
    nlohmann-json3-dev

WORKDIR /app

COPY . .
COPY plot.gnuplot /plot.gnuplot

RUN mkdir -p build && cd build && cmake .. && make -j

WORKDIR /logs

ENTRYPOINT ["/app/build/distributed_node"]
