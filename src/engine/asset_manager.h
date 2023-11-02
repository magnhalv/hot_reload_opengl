#ifndef HOT_RELOAD_OPENGL_ASSET_MANAGER_H
#define HOT_RELOAD_OPENGL_ASSET_MANAGER_H

#include <platform/types.h>
#include <cmath>
#include <cassert>
#include "logger.h"
#include "memory.h"
#include "gl_shader.h"

const i32 Max_Num_Shader_Programs = 5;

struct AssetManager {
    u64 num_shader_programs;
    GLShaderProgram shader_programs[Max_Num_Shader_Programs];
};

auto asset_manager_set_memory(void *memory) -> void;

extern AssetManager *asset_manager;

#endif //HOT_RELOAD_OPENGL_ASSET_MANAGER_H