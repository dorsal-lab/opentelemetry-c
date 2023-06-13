cmake_minimum_required(VERSION 3.1)

project(opentelemetry-c VERSION 1.0.0)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")

set(DEFAULT_BUILD_TYPE "Release")

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(opentelemetry-cpp 1.8.1 CONFIG REQUIRED)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENTELEMETRY_CPP_LIBRARIES}
        dl
        )

option(BATCH_SPAN_PROCESSOR_ENABLED "Whether to use Batching Span Processor or Simple Span Processor" OFF)
if (BATCH_SPAN_PROCESSOR_ENABLED)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BATCH_SPAN_PROCESSOR_ENABLED)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_sources(${PROJECT_NAME} PRIVATE
        src/opentelemetry_c.cpp
        src/utils/map.cpp
        src/utils/socket_carrier.cpp
        )

option(LTTNG_EXPORTER_ENABLED "Whether to use LTTng exporter or not. If set to off, OTLP GRPC default exporter will be used" ON)
if (LTTNG_EXPORTER_ENABLED)
    find_package(LTTngUST REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE lttng-ust)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LTTNG_EXPORTER_ENABLED)
    target_sources(${PROJECT_NAME} PRIVATE
            src/utils/lttng_metrics_exporter.cpp
            src/utils/lttng_opentelemetry_exporter_tracepoints.cpp
            src/utils/lttng_spans_exporter.cpp
            )
endif ()

option(BUILD_EXAMPLES "Whether to build examples or not" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1)

install(TARGETS ${PROJECT_NAME} EXPORT "${PROJECT_NAME}-target"
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES include/opentelemetry_c/opentelemetry_c.h
        DESTINATION include/opentelemetry_c)
