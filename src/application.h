#ifndef HOT_RELOAD_OPENGL_APPLICATION_H
#define HOT_RELOAD_OPENGL_APPLICATION_H

#include "types.h"
#include "../platform.h"

struct AppState {
    f32 time = 0;
};

static GLFunctions *gl;

extern "C" __declspec(dllexport) void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input);

extern "C" __declspec(dllexport) void load_gl_functions(GLFunctions * in_gl) {
    gl = in_gl;
}

#endif //HOT_RELOAD_OPENGL_APPLICATION_H
