#pragma once

#include <platform/types.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include "platform.h"
#include "asset_manager.h"
#include "camera.h"
#include "framebuffer.h"
#include "gl/gl.h"
#include "gl/gl_shader.h"
#include "gl/gl_vao.h"
#include "material.h"
#include "memory_arena.h"
#include "mesh.h"
#include "text_renderer.h"

#include "allocators/pool_allocator.h"
#include "array.h"
#include "cli.h"

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

struct EngineState {
    Pointer pointer;
    bool is_initialized = false;
    Array<Mesh> meshes;
    Camera camera;
    MemoryArena transient;
    MemoryArena permanent;

    GLGlobalUniformBufferContainer uniform_buffer_container;
    Framebuffer framebuffer;
    MultiSampleFramebuffer ms_framebuffer;
    GLVao quad_vao{};

    // TODO: Should this be global state? Might need computable buffers
    LightData light;
    Material material;
    mat4 mvp;

    // Gameplay
    PointerMode pointer_mode;
    TextRenderer text_renderer;
    Cli cli;
};

extern "C" __declspec(dllexport) void update_and_render(EngineMemory *memory, EngineInput *app_input);
extern "C" __declspec(dllexport) void load(GLFunctions * in_gl, Platform *in_platform, EngineMemory *in_memory);

