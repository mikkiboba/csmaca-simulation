cmake_minimum_required(VERSION 3.16)
project(csmaca LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# options
option(NODES "Number of nodes" 100)
option(TICKS "Number of ticks for the simulation" 500000)
option(PGEN "Packet generation probability for the nodes" 0.02)
option(EPSILON "Epsilon value for reinforcement learning" 0.1)
option(STRATEGY "Strategy used in the simulation" 0)

add_executable(csmaca
    src/Environment.cpp 
    src/Agent.cpp 
    src/Channel.cpp
    src/main.cpp
)

target_include_directories(csmaca PRIVATE include src)

target_compile_definitions(csmaca PRIVATE
    NODES=${NODES}
    TICKS=${TICKS}
    PGEN=${PGEN}
    EPSILON=${EPSILON}
    STRATEGY=${STRATEGY}
)