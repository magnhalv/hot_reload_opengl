#ifndef HOT_RELOAD_OPENGL_APPLICATION_H
#define HOT_RELOAD_OPENGL_APPLICATION_H

#include "../../platform.h"
#include "types.h"
#include "assets.h"
#include "memory.h"


struct AppState {
    bool is_initialized = false;
    f32 time = 0;
    MemoryArena transient;
};

extern GLFunctions *gl;

extern "C" __declspec(dllexport) void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input);

extern "C" __declspec(dllexport) void load_gl_functions(GLFunctions * in_gl);

#endif //HOT_RELOAD_OPENGL_APPLICATION_H
