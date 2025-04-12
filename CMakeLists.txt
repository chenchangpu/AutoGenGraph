cmake_minimum_required(VERSION 3.10)
project(Autogengraph C CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories("include")

add_subdirectory(third_party/GFLabs)

add_executable(quick_gen_data src/quick_gen_data.cpp)
