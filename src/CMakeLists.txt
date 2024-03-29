cmake_minimum_required(VERSION 3.10)   # Adjust according to your installed version

project(engine)

option(ENGINE_BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(SOURCE_FILES
        engine/array.cpp
        engine/asset_import.cpp
        engine/asset_manager.cpp

        engine/cli/cli.cpp

        engine/engine.cpp

        engine/gui.cpp

        engine/gl/gl.cpp
        engine/gl/gl_shader.cpp
        engine/gl/gl_vao.cpp

        engine/logger.cpp
        engine/memory.cpp
        engine/memory_arena.cpp
        engine/platform.cpp
        engine/ray.cpp
        engine/renderer.cpp
        engine/text_renderer.cpp
        engine/options.cpp

        engine/stb_image_write.cpp
        )

add_library(engine_dyn SHARED ${SOURCE_FILES})
add_library(engine_static ${SOURCE_FILES} )

include_directories(../shared_deps/glad/include)
include_directories(deps/src/glm)

# ASSIMP
set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")

set(ASSIMP_BUILD_OBJ_IMPORTER OFF CACHE BOOL "")
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER OFF CACHE BOOL "")

include_directories(deps/src/assimp/include)
add_subdirectory(deps/src/assimp)

include_directories(deps/src/freetype/include)
add_subdirectory(deps/src/freetype)

add_subdirectory(math)

# END ASSIMP

set(LIBS assimp math freetype)

# Dynamic configuration

target_link_libraries(engine_dyn ${LIBS})
set_target_properties(engine_dyn PROPERTIES DEBUG_POSTFIX "")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(engine_dyn PUBLIC ENGINE_DEBUG=1)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(engine_dyn PUBLIC ENGINE_DEBUG=0)
endif ()

set_target_properties(engine_dyn PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        )

# Static configuration
target_link_libraries(engine_static ${LIBS})
#set_target_properties(engine_static PROPERTIES DEBUG_POSTFIX "")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(engine_static PUBLIC ENGINE_DEBUG=1)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(engine_static PUBLIC ENGINE_DEBUG=0)
endif ()

set_target_properties(engine_dyn PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        )
