cmake_minimum_required(VERSION 3.10)   # Adjust according to your installed version

project(Application)

add_library(Application SHARED
        application/application.cpp
        application/asset_import.cpp
        application/gl_shader.cpp
        application/memory.cpp
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
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")

include_directories(deps/src/assimp/include)
add_subdirectory(deps/src/assimp)

add_subdirectory(math)

# END ASSIMP

target_link_libraries(Application assimp math)
set_target_properties(Application PROPERTIES DEBUG_POSTFIX "")

set_target_properties(Application PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)