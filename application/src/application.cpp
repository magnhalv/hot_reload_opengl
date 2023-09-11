#include <cstdio>
#include <glad/gl.h>

#include "application.h"
#include "gl_shader.h"

void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input) {
    auto *state = (AppState*)memory;

    if (!state->is_initialized) {
        //GLShader vertex("")
    }

    gl->viewport(0, 0, app_input->client_width, app_input->client_height);
    gl->clear_color(0.0f, 1.0f, 0.5f, 0.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl->enable(GL_DEPTH_TEST);

    GLenum err;
    while((err = gl->get_error()) != GL_NO_ERROR)
    {
        printf("OpenGL Error %d\n", err);
    }
}

void load_gl_functions(GLFunctions * in_gl) {
    gl = in_gl;
}