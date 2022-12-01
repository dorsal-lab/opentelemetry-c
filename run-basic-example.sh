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

echo "Starting basic example ..."
./build/examples/basic/basic-example

echo "Stop LTTng session ..."
lttng stop

echo "View traces ..."
lttng view

echo "Destroting LTTng session ..."
lttng destroy

echo "Done!"
