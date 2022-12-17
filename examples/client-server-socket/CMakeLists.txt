project(client-server-socket-example C)
set(CMAKE_C_STANDARD 11)

find_library(ZeroMQ czmq REQUIRED)

add_executable(client-server-socket-example-client)
target_link_libraries(client-server-socket-example-client PRIVATE czmq zmq opentelemetry-c)
target_sources(client-server-socket-example-client PRIVATE 
    client.c
    zhelpers.h 
)

add_executable(client-server-socket-example-server)
target_link_libraries(client-server-socket-example-server PRIVATE czmq zmq opentelemetry-c)
target_sources(client-server-socket-example-server PRIVATE 
    server.c
    zhelpers.h 
)
