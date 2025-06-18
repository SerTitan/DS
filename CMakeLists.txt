cmake_minimum_required(VERSION 3.20)
project(DistributedSimCpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(distributed_node ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(distributed_node PRIVATE Threads::Threads)
