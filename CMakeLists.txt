cmake_minimum_required(VERSION 3.14)

project(soa_vehicle_framework CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Main vehicle simulator
add_executable(
    soa_vehicle
    main.cpp
)

target_compile_options(
    soa_vehicle
    PRIVATE
    -Wall
    -Wextra
)

# -------------------------
# GoogleTest setup
# -------------------------

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable
add_executable(
    service_tests
    tests/service_tests.cpp
)

target_link_libraries(
    service_tests
    PRIVATE
    GTest::gtest_main
)

target_compile_options(
    service_tests
    PRIVATE
    -Wall
    -Wextra
)

add_executable(
    message_bus_benchmark
    benchmarks/message_bus_benchmark.cpp
)

target_compile_options(
    message_bus_benchmark
    PRIVATE
    -Wall
    -Wextra
)

include(GoogleTest)

gtest_discover_tests(service_tests)

add_executable(
    message_latency_benchmark
    benchmarks/message_latency_benchmark.cpp
)

target_compile_options(
    message_latency_benchmark
    PRIVATE
    -Wall
    -Wextra
)