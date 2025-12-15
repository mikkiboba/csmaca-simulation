cmake_minimum_required(VERSION 3.16)
project(csmaca LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# options
option(DEFAULT_NODES "Number of nodes" 100)
option(DEFAULT_TICKS "Number of ticks for the simulation" 500000)
option(DEFAULT_PGEN "Packet generation probability for the nodes" 0.02)
option(DEFAULT_CW "Minimum contention window" 16)

add_executable(csmaca
    src/main.cpp
)

target_include_directories(csmaca PRIVATE include)

target_compile_definitions(csmaca PRIVATE
    NODES=${DEFAULT_NODES}
    TICKS=${DEFAULT_TICKS}
    PGEN=${DEFAULT_PGEN}
    CW=${DEFAULT_CW}
)