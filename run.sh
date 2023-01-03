#!/usr/bin/env bash
set -e

function usage() {
    echo "Usage: ./run.sh basic|client-server-socket|up-down-counter|observable-up-down-counter"
}

case "$1" in

basic | up-down-counter | observable-up-down-counter)
    echo "Building target(s) ..."

    mkdir -p build
    cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
    cmake --build build/ --target "$1-example" --

    echo "Starting a LTTng session ..."
    lttng create "--output=ctf-traces/$1"
    lttng enable-event -u 'opentelemetry:*'
    lttng add-context -u -t vtid
    lttng start

    echo "Starting $1 example ..."
    "./build/examples/$1/$1-example"

    echo "Stop LTTng session ..."
    lttng stop

    echo "View traces ..."
    lttng view | sed 's/\(.\{400\}\).*/\1.../'

    echo "Destroying LTTng session ..."
    lttng destroy

    echo "Done!"
    ;;

client-server-socket)
    echo "Building all targets ..."

    mkdir -p build
    cmake -B build -S . -D CMAKE_BUILD_TYPE=Release
    cmake --build build/ --target client-server-socket-example-client client-server-socket-example-server --

    echo "Starting a LTTng session ..."
    lttng create --output=ctf-traces/client-server-socket
    lttng enable-event -u 'opentelemetry:*'
    lttng add-context -u -t vtid
    lttng start

    echo "Starting the server in the background ..."
    ./build/examples/client-server-socket/client-server-socket-example-server &
    SERVER_PID=$!

    echo "Starting the client ..."
    ./build/examples/client-server-socket/client-server-socket-example-client
    wait "$SERVER_PID"

    echo "Stop LTTng session ..."
    lttng stop

    echo "View traces ..."
    lttng view | sed 's/\(.\{400\}\).*/\1.../'

    echo "Destroying LTTng session ..."
    lttng destroy

    echo "Done!"
    ;;

-h | -help | --help)
    usage
    ;;

*)
    usage
    exit 1
    ;;

esac
