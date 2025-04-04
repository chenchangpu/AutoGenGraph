cmake_minimum_required(VERSION 3.10)

project(GfLabs C CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories("third_party")
include_directories("include")
include_directories("src")


add_subdirectory(src)

add_executable(gen_edges gen_edges.cpp)
target_link_libraries(gen_edges rmat_generator)

add_executable(gen_partitions gen_partitions.cpp)
target_link_libraries(gen_partitions rmat_generator)

add_executable(gen_partitions_2 gen_partitions_2.cpp)
target_link_libraries(gen_partitions rmat_generator)