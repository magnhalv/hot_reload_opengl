#include <cstdio>

#include <glad/gl.h>

#include "application.h"

#define GLAD_API_CALL_EXPORT

void update_and_render(void *memory, gl *gl) {
    int width = 1280;
    int height = 720;
    gl->viewport(0, 0, width, height);
    gl->clear_color(1.0f, 0.0f, 0.0f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl->enable(GL_DEPTH_TEST);
}