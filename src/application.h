#ifndef HOT_RELOAD_OPENGL_APPLICATION_H
#define HOT_RELOAD_OPENGL_APPLICATION_H

#include "types.h"
#include "../platform.h"

struct AppState {
    f32 time = 0;
};

extern "C" __declspec(dllexport) void update_and_render(void *memory, gl* gl);

#endif //HOT_RELOAD_OPENGL_APPLICATION_H
