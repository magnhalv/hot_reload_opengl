#ifndef HOT_RELOAD_OPENGL_APPLICATION_H
#define HOT_RELOAD_OPENGL_APPLICATION_H

#include <platform/platform.h>
#include <platform/types.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include "assets.h"
#include "memory.h"
#include "camera.h"
#include "gl_shader.h"

struct AppState {
    bool is_initialized = false;
    Mesh mesh;
    Camera camera;
    GLShaderProgram program;
    MemoryArena transient;
    GLVao vao;

    // TODO: Should this be global state? Might need computable buffers
    vec4 light;
    mat4 mvp;
};

extern GLFunctions *gl;
extern Platform *platform;

extern "C" __declspec(dllexport) void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input);
extern "C" __declspec(dllexport) void load_gl_functions(GLFunctions * in_gl);
extern "C" __declspec(dllexport) void load_platform_functions(Platform *in_platform);

#endif //HOT_RELOAD_OPENGL_APPLICATION_H
