#include <cstdio>
#include <glad/gl.h>

#include <math/vec3.h>

#include "application.h"
#include "gl_shader.h"
#include "asset_import.h"
#include "player.h"

GLFunctions *gl = nullptr;
Platform *platform = nullptr;

void update_and_render(ApplicationMemory *memory, ApplicationInput *app_input) {
    auto *state = (AppState *) memory->permanent;
    const f32 ratio = static_cast<f32>(app_input->client_width) / static_cast<f32>(app_input->client_height);

    auto *mesh = &state->mesh;
    auto &program = asset_manager->shader_programs[0];

    [[unlikely]]
    if (!state->is_initialized) {
        log_info("Initializing...");

        import_mesh("assets/meshes/cube.glb", &state->mesh);

        program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\basic_light.frag)");
        program.useProgram();

        state->camera.init(-90.0f, 0.0f, vec3(2.0f, 5.0f, 10.0f));

        // TODO: Abstract way meshes
        auto data_size = static_cast<GLsizeiptr>(sizeof(glm::vec3) * mesh->num_vertices); // NOLINT
        assert(mesh->num_vertices == mesh->num_normals);


        auto &vao = state->vao;
        vao.init();
        vao.bind();

        vao.add_buffer(mesh->vertices, data_size, 0, sizeof(vec3), 0);
        vao.add_buffer(mesh->normals, data_size, 1, sizeof(vec3), 0);
        vao.add_uniform_buffer(&state->mvp, sizeof(glm::mat4), 0, 0);
        vao.add_uniform_buffer(&state->light, sizeof(vec4), 1, 0);
        vao.load_buffers();

        state->is_initialized = true;
    }
    program.relink_if_changed();
    program.useProgram();

    gl->clear_color(1.0f, 0.6f, 0.0f, 0.0f);

    state->camera.update_cursor(static_cast<f32>(app_input->input.mouse.dx),
                                static_cast<f32>(app_input->input.mouse.dy));
    //state->camera.update_keyboard(*app_input->input);
    update_player(state, app_input);

    gl->viewport(0, 0, app_input->client_width, app_input->client_height);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    gl->enable(GL_DEPTH_TEST);

    const mat4 m = state->mesh.transform.to_mat4();
    const mat4 v = state->camera.get_view();
    const mat4 p = perspective(45.0f, ratio, 0.1f, 1000.0f);

    state->mvp = p * v * m;

    state->light = vec4(0.0f, 1.0f, 4.0f, 0.0f);

    state->vao.update_dynamic_buffers();

    gl->polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    gl->draw_arrays(GL_TRIANGLES, 0, mesh->num_vertices);

    GLenum err;
    bool found_error = false;
    while ((err = gl->get_error()) != GL_NO_ERROR) {
        printf("OpenGL Error %d\n", err);
        found_error = true;
    }
    if (found_error) {
        exit(1);
    }

    // TODO: Gotta set this on hot reload
    clear_transient();
}

void load(GLFunctions * in_gl, Platform *in_platform, ApplicationMemory *in_memory) {
    gl = in_gl;
    platform = in_platform;

    assert(sizeof(AppState) < Permanent_Memory_Block_Size);
    auto *state = (AppState *) in_memory->permanent;
    state->transient.size = Transient_Memory_Block_Size;
    state->transient.used = 0;
    state->transient.memory = (u8 *) in_memory->transient;
    set_transient_arena(&state->transient);

    assert(sizeof(AssetManager) <= Assets_Memory_Block_Size);
    asset_manager_set_memory(in_memory->asset);
}