#ifndef HOT_RELOAD_OPENGL_APPLICATION_H
#define HOT_RELOAD_OPENGL_APPLICATION_H

#ifdef BUILDING_APPLICATION_DLL
#define APPLICATION_API __declspec(dllexport)
#else
#define APPLICATION_API __declspec(dllimport)
#endif

#include "types.h"

struct AppState {
    f32 time = 0;
};

void APPLICATION_API update_and_render();

#endif //HOT_RELOAD_OPENGL_APPLICATION_H
