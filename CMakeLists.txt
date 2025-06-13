cmake_minimum_required(VERSION 3.10)
project(OceanSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(ocean_sim
    src/main.cpp
    src/ocean.cpp
    src/algae.cpp
    src/herbivore.cpp
    src/predator.cpp
)
