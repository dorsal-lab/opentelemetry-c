FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y software-properties-common && \
    apt-add-repository ppa:lttng/stable-2.13 && \
    apt-get update

RUN apt-get install -y wget \
    curl \
    zip \
    unzip \
    tar \
    curl \
	time \
    libcurl4-openssl-dev \
    build-essential \
    libczmq-dev \
    linux-headers-$(uname -r) \
    pkg-config \
    kmod \
    libnuma-dev \
    uuid-dev \
    libpopt-dev \
    liburcu-dev \
    libxml2-dev \
    babeltrace2 \
    numactl \
    binutils \
    libc-dev \
    libstdc++-10-dev \
    gcc-10 \
    g++-10 \
    clangd-12 \
    clang-tidy \
    clang-format \
    cmake \
    git \
    make \
    ninja-build

ENV CC=gcc-10 CXX=g++-10

# LTTng-UST 2.13
WORKDIR /opt/lttng
RUN cd $(mktemp -d) && \
	wget https://lttng.org/files/lttng-ust/lttng-ust-latest-2.13.tar.bz2 && \
	tar -xf lttng-ust-latest-2.13.tar.bz2 && \
	cd lttng-ust-2.13.* && \
	./configure && \
	make -j $(nproc) && \
	make -j $(nproc) install && \
	ldconfig

# LTTng-tools 2.13
WORKDIR /opt/lttng
RUN cd $(mktemp -d) && \
	wget https://lttng.org/files/lttng-tools/lttng-tools-latest-2.13.tar.bz2 && \
	tar -xf lttng-tools-latest-2.13.tar.bz2 && \
	cd lttng-tools-2.13.* && \
	./configure && \
	make -j $(nproc) && \
	make -j $(nproc) install && \
	ldconfig

# vcpkg
WORKDIR /opt/microsoft
RUN git clone -b 2022.08.15 --depth=1 --recursive https://github.com/microsoft/vcpkg.git
WORKDIR /opt/microsoft/vcpkg
RUN ./bootstrap-vcpkg.sh

# opentelemetry-cpp
WORKDIR /opt/microsoft/vcpkg
RUN ./vcpkg install nlohmann-json protobuf grpc "opentelemetry-cpp[otlp]"
ENV VCPKG_ROOT=/opt/microsoft/vcpkg

WORKDIR /code
COPY . .

CMD ./run-basic-example.sh
