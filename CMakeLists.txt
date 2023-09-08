cmake_minimum_required(VERSION 3.22)
project(hot_reload_opengl)

add_subdirectory(src)

set(CMAKE_CXX_STANDARD 20)

####### DEPENDENCIES ########
# GLAD
include_directories(deps/glad/include)
set(GLAD_SOURCES deps/glad/src/gl.c deps/glad/src/vulkan.c)
if(WIN32)
    set(GLAD_SOURCES ${GLAD_SOURCES} deps/glad/src/wgl.c)
endif()

add_library(glad ${GLAD_SOURCES})

#GLM
include_directories(deps/glm)
####### END DEPENDENCIES ########

add_executable(hot_reload_opengl win_main.cpp)

target_link_libraries(hot_reload_opengl opengl32 glad)

#target_link_libraries(hot_reload_opengl Application)