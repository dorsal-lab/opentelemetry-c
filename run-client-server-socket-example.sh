#!/usr/bin/env bash
set -ex

echo "Building all targets ..."

mkdir -p build
cmake -B build
cmake --build build/ --config Debug --target all --

echo "Starting a LTTng session ..."
lttng create --output=ctf-traces/
lttng enable-event -u 'opentelemetry:*'
lttng add-context -u -t vtid
lttng start

echo "Starting the server in the background ..."
./build/examples/client-server-socket/client-server-socket-example-server&
SERVER_PID=$!

echo "Starting the client ..."
./build/examples/client-server-socket/client-server-socket-example-client
wait "$SERVER_PID"

echo "Stop LTTng session ..."
lttng stop

echo "View traces ..."
lttng view

echo "Destroting LTTng session ..."
lttng destroy

echo "Done!"
