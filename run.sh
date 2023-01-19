#!/usr/bin/env bash
set -e

function usage() {
    echo "Usage: ./run.sh basic|client-server-socket|up-down-counter|observable-up-down-counter"
}

build_dir=/tmp/opentelemetry-c-build
mkdir -p "$build_dir"

case "$1" in

basic | up-down-counter | observable-up-down-counter)
    echo "Building $1-example target ..."

    cmake -B "$build_dir" -S . -D CMAKE_BUILD_TYPE=Release -D BUILD_EXAMPLES=ON
    cmake --build "$build_dir" --target "$1-example" --

    echo "Starting a LTTng session ..."
    lttng create "--output=ctf-traces/$1"
    lttng enable-event -u 'opentelemetry:*'
    lttng add-context -u -t vtid
    lttng start

    echo "Starting $1 example ..."
    "$build_dir/examples/$1/$1-example"

    echo "Stop LTTng session ..."
    lttng stop

    echo "View traces ..."
    lttng view | sed 's/\(.\{400\}\).*/\1.../'

    echo "Destroying LTTng session ..."
    lttng destroy

    echo "Done!"
    ;;

client-server-socket)
    echo "Building client-server-socket-example-client and client-server-socket-example-server targets ..."

    cmake -B "$build_dir" -S . -D CMAKE_BUILD_TYPE=Release -D BUILD_EXAMPLES=ON
    cmake --build "$build_dir" --target client-server-socket-example-client client-server-socket-example-server --

    echo "Starting a LTTng session ..."
    lttng create --output=ctf-traces/client-server-socket
    lttng enable-event -u 'opentelemetry:*'
    lttng add-context -u -t vtid
    lttng start

    echo "Starting the server in the background ..."
    "$build_dir/examples/client-server-socket/client-server-socket-example-server" &
    SERVER_PID=$!

    echo "Starting the client ..."
    "$build_dir/examples/client-server-socket/client-server-socket-example-client"

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
