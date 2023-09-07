cmake_minimum_required(VERSION 3.22)
project(hot_reload_opengl)

add_subdirectory(src)

set(CMAKE_CXX_STANDARD 20)

add_executable(hot_reload_opengl win_main.cpp)

target_link_libraries(hot_reload_opengl Application)

set_target_properties(hot_reload_opengl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
)