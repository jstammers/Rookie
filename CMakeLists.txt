
cmake_minimum_required(VERSION 3.0.0)
set(CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS_RELEASE "-O2")
project(Rookie VERSION 1.1)

add_subdirectory(src)
enable_testing()
add_subdirectory(test)