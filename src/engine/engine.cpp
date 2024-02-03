#include <cstdio>
#include <glad/gl.h>

#include <math/vec3.h>

#include "engine.h"
#include "asset_import.h"
#include "array.h"
#include "ray.h"
#include "renderer.h"

// Globals
GraphicsOptions *graphics_options = nullptr;

auto Pointer::update_pos(const MouseRaw &raw, i32 client_width, i32 client_height) -> void {
    // TODO: Sensitivity must be moved somewhere else
    const f32 sensitivity = 2.0;
    const f32 dx = static_cast<f32>(raw.dx) * sensitivity;
    const f32 dy = static_cast<f32>(raw.dy) * sensitivity;

    x = min(max(dx + x, 0.0f), static_cast<f32>(client_width));
    y = min(max(dy + y, 0.0f), static_cast<f32>(client_height));
}

auto Pointer::update_ray(const mat4 &view, const mat4 &inv_projection, i32 client_width, i32 client_height) -> void {
    mat4 inv_view_matrix = inverse(view);

    f32 normalized_x = ((2.0f * static_cast<f32>(x)) / static_cast<f32>(client_width)) - 1.0f;
    f32 normalized_y = ((2.0f * static_cast<f32>(y)) / static_cast<f32>(client_height)) - 1.0f;

    vec4 mouse_pos = vec4(normalized_x, normalized_y, -1.0f, 1.f);

    vec4 mouse_eye_coords = inv_projection * mouse_pos;
    mouse_eye_coords.z = -1.0f;
    mouse_eye_coords.w = 0.0f;

    ray = normalized(to_vec3(inv_view_matrix * mouse_eye_coords));
}


inline auto to_ndc(i32 pixels, i32 range) -> f32 {
    auto x = range / 2;
    return (static_cast<f32>(pixels) / static_cast<f32>(x)) - 1;
}

inline f32 new_x(f32 x, f32 y, f32 degree) {
    return cos(degree) * x - sin(degree) * y;
}

inline f32 new_y(f32 x, f32 y, f32 degree) {
    return sin(degree) * x + cos(degree) * y;
}

void update_and_render(EngineMemory *memory, EngineInput *app_input) {
    auto *state = (EngineState *) memory->permanent;
    const f32 ratio = static_cast<f32>(app_input->client_width) / static_cast<f32>(app_input->client_height);

    auto &mesh_program = asset_manager->shader_programs[0];
    auto &single_color_mesh_program = asset_manager->shader_programs[1];
    auto &single_color_program = asset_manager->shader_programs[2];
    auto &quad_program = asset_manager->shader_programs[3];
    auto &font_program = asset_manager->shader_programs[4];
    asset_manager->num_shader_programs = 5;

    // region Initialize
    [[unlikely]]
    if (!state->is_initialized) {
        log_info("Initializing...");

        state->permanent.init(
                static_cast<u8 *>(memory->permanent) + sizeof(EngineState),
                Permanent_Memory_Block_Size - sizeof(EngineState));

        state->camera.init(-90.0f, -27.0f, vec3(0.0f, 5.0f, 10.0f));

        state->pointer.x = 200;
        state->pointer.y = 200;


        // Load in meshes
        const int num_meshes = 3;
        state->meshes.init(static_cast<Mesh *>(state->permanent.allocate(sizeof(Mesh) * num_meshes)), num_meshes);

        import_mesh("assets/meshes/asset_Cube.fbx", &state->meshes[0]);
        state->meshes[0].id = 1;
        state->meshes[0].transform.position.x = 0;
        state->meshes[0].transform.position.y = 0;
        state->meshes[0].transform.position.z = 0;
        import_mesh("assets/meshes/asset_Sphere.fbx", &state->meshes[1]);
        state->meshes[1].transform.position.x = -10;
        state->meshes[1].id = 2;
        import_mesh("assets/meshes/asset_Suzanne.fbx", &state->meshes[2]);
        state->meshes[2].transform.position.x = 10;
        state->meshes[2].id = 3;

        state->framebuffer.init(app_input->client_width, app_input->client_height);
        state->ms_framebuffer.init(app_input->client_width, app_input->client_height);
        // region Compile shaders
        mesh_program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\phong.frag)");
        single_color_mesh_program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\single_color.frag)");
        single_color_mesh_program.useProgram();
        single_color_mesh_program.set_uniform("color", vec4(0.7f, 0.1f, 0.2f, 0.0f));
        single_color_program.initialize(R"(.\assets\shaders\basic_2d.vert)", R"(.\assets\shaders\single_color.frag)");
        quad_program.initialize(R"(.\assets\shaders\quad.vert)", R"(.\assets\shaders\quad.frag)");

        state->uniform_buffer_container.add(&state->mvp, sizeof(mat4), 0, 0);
        state->uniform_buffer_container.add(&state->light, sizeof(LightData), 1, 0);
        state->uniform_buffer_container.add(&state->material, sizeof(Material), 2, 0);

        // endregion

        // TODO: Handle change of screen width and height

        for (auto &mesh: state->meshes) {
            mesh.vao.init();
            mesh.vao.bind();

            auto data_size = static_cast<GLsizeiptr>(sizeof(vec3) * mesh.num_vertices); // NOLINT
            assert(mesh.num_vertices == mesh.num_normals);

            // TODO: Should be packed into a single buffer
            //mesh.vao.add_buffer(data_size, 3, sizeof(vec3), 0, 0);
            mesh.vao.add_buffer(data_size);
            mesh.vao.add_buffer_desc(0, 0, 3, 0, sizeof(vec3));
            mesh.vao.add_buffer(data_size);
            mesh.vao.add_buffer_desc(1, 1, 3, 0, sizeof(vec3));
            mesh.vao.upload_buffer_desc();

            mesh.vao.upload_buffer_data(0, mesh.vertices, 0, data_size);
            mesh.vao.upload_buffer_data(1, mesh.normals, 0, data_size);
        }

        float quad_verticies[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
                // positions   // texCoords
                -1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,

                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f
        };

        state->quad_vao.init();
        state->quad_vao.bind();
        state->quad_vao.add_buffer(sizeof(quad_verticies));
        state->quad_vao.add_buffer_desc(0, 0, 2, 0, 4*sizeof(f32));
        state->quad_vao.add_buffer_desc(0, 1, 2, 2 * sizeof(f32), 4*sizeof(f32));
        state->quad_vao.upload_buffer_desc();
        state->quad_vao.upload_buffer_data(0, quad_verticies, 0, sizeof(quad_verticies));

        state->camera.update_cursor(0, 0);

        auto cli_memory_arena = state->permanent.allocate_arena(MegaBytes(1));
        state->cli.init(&single_color_program, &font_program, cli_memory_arena);

        state->is_initialized = true;
    }

    // endregion

#if ENGINE_DEBUG
    state->permanent.check_integrity();
    asset_manager->update_if_changed();
#endif

    state->material = get_material("metal");

    const auto projection = perspective(45.0f, ratio, 0.1f, 100.0f);
    const auto ortho_projection = create_ortho(0, app_input->client_width, 0, app_input->client_height, 0.0f, 100.0f);
    const auto inv_projection = inverse(projection);
    const auto view = state->camera.get_view();
    const auto pv = projection * view;

    Pointer *pointer = &state->pointer;
    const MouseRaw *mouse = &app_input->input.mouse_raw;


    Mesh *hovered_mesh = nullptr;
    for (auto &mesh: state->meshes) {
        // TODO: Check which on is in front
        vec2 intersections;
        if (intersects(state->camera.get_position(), pointer->ray, mesh, intersections)) {
            hovered_mesh = &mesh;
        }
    }

    switch (state->pointer_mode) {
        case PointerMode::NORMAL:
            if (mouse->left.is_pressed_this_frame()) {
                if (hovered_mesh == nullptr) {
                    state->pointer_mode = PointerMode::LOOK_AROUND;
                } else {
                    state->pointer_mode = PointerMode::GRAB;
                }
            }
            break;
        case PointerMode::LOOK_AROUND:
        case PointerMode::GRAB: {
            if (mouse->left.is_released_this_frame()) {
                state->pointer_mode = PointerMode::NORMAL;
            }
        }
            break;
    }

    if (state->is_cli_enabled) {
        state->cli.handle_input(&app_input->input);
    }
    else {
        if (state->pointer_mode == PointerMode::NORMAL || state->pointer_mode == PointerMode::GRAB) {
            state->pointer.update_pos(*mouse, app_input->client_width, app_input->client_height);
        } else {
            state->camera.update_cursor(static_cast<f32>(mouse->dx), static_cast<f32>(mouse->dy));
        }
        state->pointer.update_ray(view, inv_projection, app_input->client_width, app_input->client_height);
    }


    Mesh floor;
    floor.num_vertices = 4;
    floor.vertices[0] = vec3(F32_MAX, 0, F32_MAX);
    floor.vertices[1] = vec3(-F32_MAX, 0, F32_MAX);
    floor.vertices[2] = vec3(-F32_MAX, 0, -F32_MAX);
    floor.vertices[3] = vec3(F32_MAX, 0, -F32_MAX);

    vec2 floor_intersections;
    if (intersects(state->camera.get_position(), pointer->ray, floor, floor_intersections)
        && state->pointer_mode == PointerMode::GRAB
        && hovered_mesh != nullptr) {
        vec3 location = (pointer->ray * floor_intersections.x) + state->camera.get_position();
        hovered_mesh->transform.position = location;
    }

    const Light light = {
            .position_ws = vec3(0, 2.0f, 2.0f),
            .radius = 20.0f,
            .color = vec3(1.0, 1.0, 1.0),
    };

    // region Render setup
    if (graphics_options->anti_aliasing) {
        gl->bind_framebuffer(GL_FRAMEBUFFER, state->ms_framebuffer.fbo);
    }
    else {
        gl->bind_framebuffer(GL_FRAMEBUFFER, state->framebuffer.fbo);
    }

    gl->enable(GL_DEPTH_TEST);
    gl->clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl->viewport(0, 0, app_input->client_width, app_input->client_height);
    // endregion

    // region Render
    mesh_program.useProgram();
    for (auto &mesh: state->meshes) {
        if (hovered_mesh != nullptr && mesh.id == hovered_mesh->id && state->pointer_mode != PointerMode::LOOK_AROUND) {
            continue;
        }

        const mat4 m = mesh.transform.to_mat4();
        state->light = light.to_data(inverse(m), state->camera.get_position());
        state->mvp = pv * m;

        mesh.vao.bind();
        state->uniform_buffer_container.upload();

        gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }

    if (hovered_mesh != nullptr && state->pointer_mode != PointerMode::LOOK_AROUND) {
        enable_stencil_test();

        const mat4 m = hovered_mesh->transform.to_mat4();
        state->light = light.to_data(inverse(m), state->camera.get_position());
        state->mvp = pv * m;

        hovered_mesh->vao.bind();
        state->uniform_buffer_container.upload();
        gl->draw_arrays(GL_TRIANGLES, 0, hovered_mesh->num_vertices);

        enable_outline();

        single_color_mesh_program.useProgram();

        hovered_mesh->vao.bind();
        auto t = hovered_mesh->transform;
        t.scale.x = 1.1;
        t.scale.y = 1.1;
        t.scale.z = 1.1;

        state->mvp = pv * t.to_mat4();
        state->uniform_buffer_container.upload();
        gl->draw_arrays(GL_TRIANGLES, 0, hovered_mesh->num_vertices);

        disable_stencil_test();
    }

    // region Draw text
    const f32 c_width = static_cast<f32>(app_input->client_width);
    const f32 c_height = static_cast<f32>(app_input->client_height);
    if (app_input->input.oem_5.is_pressed_this_frame()) {
        state->is_cli_enabled = state->cli.toggle(c_height);
    }

    state->cli.update(c_width, c_height, app_input->dt);
    state->cli.render_background(c_width, c_height);
    state->cli.render_text(c_width, c_height);

    // endregion


    // region Draw pointer
    if (state->pointer_mode != PointerMode::LOOK_AROUND) {
        single_color_program.useProgram();
        single_color_program.set_uniform("color", vec4(0.7f, 0.7f, 0.7f, 0.7f));
        single_color_program.set_uniform("projection", ortho_projection);
        // TODO Fix this, worst implementation ever
        f32 x = state->pointer.x;
        f32 y = state->pointer.y;
        f32 cursor_vertices[6] = {
                x, y,
                x + new_x(-10.0f, -20.0f, 45.0f), y + new_y( - 10,  - 20, 45.0f),
                x + new_x(10.0f, -20.0f, 45.0f), y+ new_y(10, - 20, 45.0f),
        };
        GLVao cursor_vao{};
        cursor_vao.init();
        cursor_vao.bind();
        cursor_vao.add_buffer(sizeof(cursor_vertices));
        cursor_vao.add_buffer_desc(0, 0, 2, 0, sizeof(vec2));
        cursor_vao.upload_buffer_desc();
        cursor_vao.upload_buffer_data(0, cursor_vertices, 0, sizeof(cursor_vertices));

        gl->draw_arrays(GL_TRIANGLES, 0, 3);
        cursor_vao.destroy();
    }
    // endregion

    // region Draw end quad
    {
        if (graphics_options->anti_aliasing) {
            const i32 w = app_input->client_width;
            const i32 h = app_input->client_height;
            gl->bind_framebuffer(GL_READ_FRAMEBUFFER, state->ms_framebuffer.fbo);
            gl->bind_framebuffer(GL_DRAW_FRAMEBUFFER, state->framebuffer.fbo);
            gl->framebuffer_blit(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        state->quad_vao.bind();
        quad_program.useProgram();
        gl->bind_framebuffer(GL_FRAMEBUFFER, 0);
        gl->clear_color(0.0f, 0.0f, 0.0f, 1.0f);
        gl->clear(GL_COLOR_BUFFER_BIT);
        gl->disable(GL_DEPTH_TEST);

        gl->texture_bind(GL_TEXTURE_2D, state->framebuffer.texture);
        gl->draw_arrays(GL_TRIANGLES, 0, 6);

        gl->texture_bind(GL_TEXTURE_2D, 0);
    }
    // endregion

    // endregion

    gl->use_program(0);
    gl->bind_vertex_array(0);

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
    load_gl(in_gl);
    platform = in_platform;

    assert(sizeof(EngineState) < Permanent_Memory_Block_Size);
    auto *state = (EngineState *) in_memory->permanent;
    state->transient.init(in_memory->transient, Transient_Memory_Block_Size);
    set_transient_arena(&state->transient);

    assert(sizeof(AssetManager) <= Assets_Memory_Block_Size);
    asset_manager_set_memory(in_memory->asset);

    graphics_options = &state->graphics_options;
}