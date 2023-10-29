#ifndef HOT_RELOAD_OPENGL_ENGINE_H
#define HOT_RELOAD_OPENGL_ENGINE_H

#include <platform/platform.h>
#include <platform/types.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include "assets.h"
#include "memory_arena.h"
#include "camera.h"
#include "gl_shader.h"
#include "asset_manager.h"

#include "allocators/pool_allocator.h"

struct EngineState {
    bool is_initialized = false;
    Mesh mesh;
    Camera camera;
    MemoryArena transient;
    GLVao vao;

    PoolAllocator pool;

    // TODO: Should this be global state? Might need computable buffers
    vec4 light;
    mat4 mvp;
};

extern GLFunctions *gl;
extern Platform *platform;

extern "C" __declspec(dllexport) void update_and_render(EngineMemory *memory, EngineInput *app_input);
extern "C" __declspec(dllexport) void load(GLFunctions * in_gl, Platform *in_platform, EngineMemory *in_memory);

#endif //HOT_RELOAD_OPENGL_ENGINE_H
