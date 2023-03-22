FROM ghcr.io/augustinsangam/lttng-otelcpp:main

RUN ldconfig

WORKDIR /tmp/opentelemetry-c
COPY . .
RUN mkdir -p build &&\
    cd build &&\
    cmake -DWITH_EXAMPLES=OFF \
        -DBUILD_SHARED_LIBS=ON \
        -DCMAKE_INSTALL_PREFIX=/usr/local/ \
        .. &&\
    make -j $(nproc) &&\
    make install

CMD ./run.sh basic && \
	./run.sh up-down-counter && \
	./run.sh observable-up-down-counter && \
	./run.sh client-server-socket
