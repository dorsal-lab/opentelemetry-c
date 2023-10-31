FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu-22.04

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
	linux-headers-generic \
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

# Install grpc 1.48.1
WORKDIR /tmp
RUN git clone --recurse-submodules "-j$(nproc)" -b v1.48.1 --depth 1 --shallow-submodules https://github.com/grpc/grpc
RUN cd grpc &&\
	mkdir -p build &&\
	cd build &&\
	cmake -DgRPC_INSTALL=ON \
		-DgRPC_BUILD_TESTS=OFF \
		-DCMAKE_INSTALL_PREFIX=/usr/local/ \
		.. &&\
	make -j $(nproc) &&\
	make install

# Install opentelemetry 1.8.1
WORKDIR /tmp
RUN git clone --recurse-submodules "-j$(nproc)" -b v1.8.1 --depth 1 --shallow-submodules https://github.com/open-telemetry/opentelemetry-cpp.git
WORKDIR /tmp/opentelemetry-cpp
COPY protobuf_libraries_as_private_dependencies.patch .
RUN git apply protobuf_libraries_as_private_dependencies.patch
RUN	mkdir -p build &&\
	cd build &&\
	cmake -DBUILD_TESTING=OFF \
		-DWITH_BENCHMARK=OFF \
		-DWITH_EXAMPLES=OFF \
        -DWITH_LOGS_PREVIEW=ON \
		-DWITH_OTLP=ON \
		-DWITH_OTLP_GRPC=ON \
        -DWITH_OTLP_HTTP=OFF \
		-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_INSTALL_PREFIX=/usr/local/ \
		.. &&\
	make -j $(nproc) &&\
	make install

RUN ldconfig
