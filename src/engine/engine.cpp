#include <cstdio>
#include <glad/gl.h>

#include <math/vec3.h>

#include "engine.h"
#include "gl_shader.h"
#include "asset_import.h"
#include "player.h"

GLFunctions *gl = nullptr;
Platform *platform = nullptr;

const i32 meshes_to_draw = 3;

void update_and_render(EngineMemory *memory, EngineInput *app_input) {
    auto *state = (EngineState *) memory->permanent;
    const f32 ratio = static_cast<f32>(app_input->client_width) / static_cast<f32>(app_input->client_height);

    auto &program = asset_manager->shader_programs[0];
    auto &single_color = asset_manager->shader_programs[1];
    asset_manager->num_shader_programs = 2;

    [[unlikely]]
    if (!state->is_initialized) {
        log_info("Initializing...");

        import_mesh("assets/meshes/asset_Cube.fbx", &state->meshes[0]);
        import_mesh("assets/meshes/asset_Sphere.fbx", &state->meshes[1]);
        state->meshes[1].transform.position.x = -10;
        import_mesh("assets/meshes/asset_Suzanne.fbx", &state->meshes[2]);
        state->meshes[2].transform.position.x = 10;

        program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\basic_light.frag)");
        program.add_uniform_buffer(&state->mvp, sizeof(glm::mat4), 0, 0);
        program.add_uniform_buffer(&state->light, sizeof(vec4), 1, 0);

        single_color.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\single_color.frag)");
        single_color.add_uniform_buffer(&state->mvp, sizeof(glm::mat4), 0, 0);

        state->camera.init(-90.0f, 0.0f, vec3(2.0f, 5.0f, 10.0f));

        // TODO: Abstract way meshes
        for (auto & mesh : state->meshes) {
            mesh.vao.init();
            mesh.vao.bind();

            auto data_size = static_cast<GLsizeiptr>(sizeof(glm::vec3) * mesh.num_vertices); // NOLINT
            assert(mesh.num_vertices == mesh.num_normals);

            mesh.vao.add_buffer(mesh.vertices, data_size, 0, sizeof(vec3), 0);
            mesh.vao.add_buffer(mesh.normals, data_size, 1, sizeof(vec3), 0);
            mesh.vao.load_buffers();
        }

        state->arena.init(static_cast<u8 *>(memory->permanent) + sizeof(EngineState),
                          Permanent_Memory_Block_Size - sizeof(EngineState));

        state->is_initialized = true;
    }

    state->arena.check_integrity();
    asset_manager->update_if_changed();

    // Start application
    program.useProgram();

    gl->clear_color(1.0f, 0.6f, 0.0f, 0.0f);

    state->camera.update_cursor(static_cast<f32>(app_input->input.mouse.dx),
                                static_cast<f32>(app_input->input.mouse.dy));
    //state->camera.update_keyboard(*app_input->input);
    update_player(state, app_input);

    gl->viewport(0, 0, app_input->client_width, app_input->client_height);

    gl->enable(GL_DEPTH_TEST);
    gl->enable(GL_STENCIL_TEST);
    gl->stencil_mask(0xFF);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    gl->stencil_func(GL_ALWAYS, 1, 0xFF);
    gl->stencil_op(GL_KEEP, GL_KEEP, GL_REPLACE);

    for (auto & mesh : state->meshes) {
        mesh.vao.bind();
        const mat4 m = mesh.transform.to_mat4();
        const mat4 v = state->camera.get_view();
        const mat4 p = perspective(45.0f, ratio, 0.1f, 100.0f);

        state->mvp = p * v * m;
        state->light = vec4(0.0f, 1.0f, 5.0f, 0.0f);

        program.update_dynamic_buffers();

        gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }

    gl->stencil_func(GL_NOTEQUAL, 1, 0xFF);
    gl->stencil_mask(0x00);
    gl->disable(GL_DEPTH_TEST);
    single_color.useProgram();
    for (auto & mesh : state->meshes) {
        mesh.vao.bind();
        auto t = mesh.transform;
        t.scale.x = 1.1;
        t.scale.y = 1.1;
        t.scale.z = 1.1;
        const mat4 m = t.to_mat4();
        const mat4 v = state->camera.get_view();
        const mat4 p = perspective(45.0f, ratio, 0.1f, 100.0f);
        state->mvp = p * v * m;

        single_color.update_dynamic_buffers();

        gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }

    gl->disable(GL_STENCIL_TEST);
    gl->enable(GL_DEPTH_TEST);

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

void load(GLFunctions *in_gl, Platform *in_platform, EngineMemory *in_memory) {
    gl = in_gl;
    platform = in_platform;

    assert(sizeof(EngineState) < Permanent_Memory_Block_Size);
    auto *state = (EngineState *) in_memory->permanent;
    state->transient.size = Transient_Memory_Block_Size;
    state->transient.used = 0;
    state->transient.memory = (u8 *) in_memory->transient;
    set_transient_arena(&state->transient);

    assert(sizeof(AssetManager) <= Assets_Memory_Block_Size);
    asset_manager_set_memory(in_memory->asset);
}