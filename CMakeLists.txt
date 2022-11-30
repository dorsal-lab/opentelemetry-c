cmake_minimum_required(VERSION 3.22.1)

# Setup vcpkg script with CMake (note: should be placed before project() call)
if(NOT DEFINED ENV{VCPKG_ROOT})
 	set(VCPKG_ROOT "/opt/microsoft/vcpkg")
else()
	set(VCPKG_ROOT $ENV{VCPKG_ROOT})
endif()
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(opentelemetry-c)

set(CMAKE_CXX_STANDARD 11)

find_package(CURL)
find_package(LTTngUST REQUIRED)
find_package(opentelemetry-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# target = opentelemetry-c
add_library(opentelemetry-c)
target_include_directories(opentelemetry-c PRIVATE
    ${OPENTELEMETRY_CPP_INCLUDE_DIRS}
    ${LTTNGUST_CPP_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIRS}
    ${PROTOBUF_CPP_INCLUDE_DIRS}
    ${GRPC_CPP_INCLUDE_DIRS})
target_link_libraries(opentelemetry-c PRIVATE
    ${OPENTELEMETRY_CPP_LIBRARIES}
    ${LTTNGUST_CPP_LIBRARIES}
    ${NLOHMANN_JSON_CPP_LIBRARIES}
    ${PROTOBUF_CPP_CPP_LIBRARIES}
    ${GRPC_CPP_CPP_LIBRARIES}
    lttng-ust
    dl)

target_include_directories(opentelemetry-c PUBLIC include)
target_sources(opentelemetry-c PRIVATE
	src/opentelemetry-c.cpp
    src/utils/lttng_span_exporter.cpp
    src/utils/map.cpp
    src/utils/socket_carrier.cpp)

# Include examples
# TODO(augustinsangam): Change to OFF
option(BUILD_EXAMPLES "Whether to build examples or not" ON)
if (BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
