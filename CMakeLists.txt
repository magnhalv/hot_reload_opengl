cmake_minimum_required(VERSION 3.22)
project(hot_reload_opengl)

add_subdirectory(src)

set(CMAKE_CXX_STANDARD 20)

add_executable(hot_reload_opengl win_main.cpp)

#target_link_libraries(hot_reload_opengl Application)