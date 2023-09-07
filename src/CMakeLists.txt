cmake_minimum_required(VERSION 3.10)   # Adjust according to your installed version

add_library(Application SHARED application.cpp)

set_target_properties(Application PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
)