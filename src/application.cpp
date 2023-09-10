#include <cstdio>
#include <glad/gl.h>

#include "application.h"

void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input) {
    gl->viewport(0, 0, app_input->client_width, app_input->client_height);
    gl->clear_color(1.0f, 0.5f, 0.5f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl->enable(GL_DEPTH_TEST);

    GLenum err;
    while((err = gl->get_error()) != GL_NO_ERROR)
    {
        printf("OpenGL Error %d\n", err);
    }
}