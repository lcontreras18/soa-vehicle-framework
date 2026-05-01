cmake_minimum_required(VERSION 3.14)
project(soa_vehicle_framework CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(soa_vehicle main.cpp)

target_compile_options(soa_vehicle PRIVATE -Wall -Wextra)
