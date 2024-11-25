cmake_minimum_required(VERSION 3.15)
project(MPI_Project)

find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 17)
include_directories(${MPI_INCLUDE_PATH})

add_executable(MPI_Project main.cpp data_generator.cpp)
target_link_libraries(MPI_Project ${MPI_LIBRARIES})
