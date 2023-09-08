cmake_minimum_required(VERSION 3.10)   # Adjust according to your installed version

project(Application)

add_library(Application SHARED application.cpp)

include_directories(../deps/glad/include)

target_link_libraries(Application)

set_target_properties(Application PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
)