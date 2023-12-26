cmake_minimum_required(VERSION 3.10)   # Adjust according to your installed version

project(Engine)

add_library(Engine SHARED
        engine/array.cpp
        engine/asset_import.cpp
        engine/asset_manager.cpp
        engine/engine.cpp
        engine/gl_shader.cpp
        engine/memory.cpp
        engine/memory_arena.cpp
        engine/ray.cpp
        engine/renderer.cpp
        engine/gl.cpp
        )

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

add_subdirectory(math)

# END ASSIMP

target_link_libraries(Engine assimp math)
set_target_properties(Engine PROPERTIES DEBUG_POSTFIX "")

set_target_properties(Engine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/app
)