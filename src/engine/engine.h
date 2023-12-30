#pragma once

#include <platform/platform.h>
#include <platform/types.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include "asset_manager.h"
#include "camera.h"
#include "framebuffer.h"
#include "gl.h"
#include "gl_shader.h"
#include "material.h"
#include "memory_arena.h"
#include "mesh.h"

#include "allocators/pool_allocator.h"
#include "array.h"

enum class PointerMode {
    NORMAL = 0,
    LOOK_AROUND,
    GRAB
};

/// Processed mouse input
struct Pointer {
    f32 x = 0;
    f32 y = 0;

    vec3 ray; // Mouse ray in world space from the camera enabled at the start of the frame
    auto update_pos(const MouseRaw &raw, i32 client_width, i32 client_height) -> void;
    auto update_ray(const mat4 &view, const mat4 &inv_projection, i32 client_width, i32 client_height) -> void;
};

struct Character {
    u32 texture_id{};
    ivec2 size;
    ivec2 bearing;
    u32 advance{};
};

struct EngineState {
    Pointer pointer;
    bool is_initialized = false;
    Array<Mesh> meshes;
    Camera camera;
    MemoryArena transient;
    MemoryArena permanent;

    Framebuffer framebuffer;
    MultiSampleFramebuffer ms_framebuffer;
    GLVao quad_vao{};

    // TODO: Should this be global state? Might need computable buffers
    LightData light;
    Material material;
    mat4 mvp;

    // Gameplay
    PointerMode pointer_mode;
    Array<Character> font;
};

extern Platform *platform;

extern "C" __declspec(dllexport) void update_and_render(EngineMemory *memory, EngineInput *app_input);
extern "C" __declspec(dllexport) void load(GLFunctions * in_gl, Platform *in_platform, EngineMemory *in_memory);

