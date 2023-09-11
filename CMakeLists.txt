cmake_minimum_required(VERSION 3.22)
project(hot_reload_opengl)

set(CMAKE_CXX_STANDARD 20)

####### DEPENDENCIES ########
# GLAD
include_directories(shared_deps/glad/include)
set(GLAD_SOURCES shared_deps/glad/src/gl.c shared_deps/glad/src/vulkan.c)
if(WIN32)
    set(GLAD_SOURCES ${GLAD_SOURCES} shared_deps/glad/src/wgl.c)
endif()


find_package(OpenGL REQUIRED)

####### END DEPENDENCIES ########

add_executable(hot_reload_opengl
        win_main.cpp
        ${GLAD_SOURCES}
        )

target_link_libraries(hot_reload_opengl ${OPENGL_gl_LIBRARY} )

# This needs to be after the linking of opengl, otherwise we get linking issues against glad
# Something related to assimp.
add_subdirectory(application)
