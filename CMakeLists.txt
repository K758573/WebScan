cmake_minimum_required(VERSION 3.25)
project(WebScan)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src)
link_libraries(utils)
add_executable(WebScan main.cpp)