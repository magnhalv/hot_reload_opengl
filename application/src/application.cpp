#include <cstdio>
#include <glad/gl.h>

#include "application.h"
#include "gl_shader.h"

GLFunctions *gl = nullptr;

void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input) {
    auto *state = (AppState*)memory->permanent_storage;

    if (!state->is_initialized) {
        GLShader vertex(R"(.\assets\shaders\basic.vert)");
        GLShader frag(R"(.\assets\shaders\basic.frag)");
        GLProgram program;
        program.initialize(vertex, frag);

        state->transient.size = memory->transient_storage_size;
        state->transient.memory = (u8*)memory->transient_storage;
        set_transient_arena(&state->transient);

        state->is_initialized = true;
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