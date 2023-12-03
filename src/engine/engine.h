#pragma once

#include <platform/platform.h>
#include <platform/types.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include "mesh.h"
#include "memory_arena.h"
#include "camera.h"
#include "gl_shader.h"
#include "asset_manager.h"

#include "allocators/pool_allocator.h"
#include "array.h"

struct EngineState {
    bool is_initialized = false;
    Array<Mesh> meshes;
    Camera camera;
    MemoryArena transient;
    MemoryArena permanent;


    // TODO: Should this be global state? Might need computable buffers
    vec4 light;
    mat4 mvp;
};

extern GLFunctions *gl;
extern Platform *platform;

extern "C" __declspec(dllexport) void update_and_render(EngineMemory *memory, EngineInput *app_input);
extern "C" __declspec(dllexport) void load(GLFunctions * in_gl, Platform *in_platform, EngineMemory *in_memory);

