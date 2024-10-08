#include <cmath>
#include <cstddef>
#include <cstdio>
#include <glad/gl.h>

#include <math/vec3.h>
#include <math/math.h>

#include "array.h"
#include "asset_import.h"
#include "asset_manager.h"
#include "engine.h"
#include "gl/gl.h"
#include "gl/gl_vao.h"
#include "gui.hpp"
#include "logger.h"
#include "material.h"
#include "math/quat.h"
#include "math/transform.h"
#include "memory_arena.h"
#include "options.hpp"
#include "platform.h"
#include "ray.h"
#include "renderer.h"
#include "text_renderer.h"

// Globals
Options* graphics_options = nullptr;

auto Pointer::update_pos(const MouseRaw& raw, i32 client_width, i32 client_height) -> void {
  // TODO: Sensitivity must be moved somewhere else
  const f32 sensitivity = 2.0;
  const f32 dx = static_cast<f32>(raw.dx) * sensitivity;
  const f32 dy = static_cast<f32>(raw.dy) * sensitivity;

  x = std::min(std::max(dx + x, 0.0f), static_cast<f32>(client_width));
  y = std::min(std::max(dy + y, 0.0f), static_cast<f32>(client_height));
}

auto Pointer::update_ray(const mat4& view, const mat4& inv_projection, i32 client_width, i32 client_height) -> void {
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

// TODO: Handle change of screen width and height
void update_and_render(EngineMemory* memory, EngineInput* app_input) {
  auto* state = (EngineState*)memory->permanent;
  const f32 ratio = static_cast<f32>(app_input->client_width) / static_cast<f32>(app_input->client_height);

  auto& mesh_program = asset_manager->shader_programs[0];
  auto& single_color_mesh_program = asset_manager->shader_programs[1];
  auto& single_color_program = asset_manager->shader_programs[2];
  auto& quad_program = asset_manager->shader_programs[3];
  auto& font_program = asset_manager->shader_programs[4];
  auto& grid_program = asset_manager->shader_programs[5];
  auto& imgui_program = asset_manager->shader_programs[6];
  asset_manager->num_shader_programs = 7;

  // region Initialize
  [[unlikely]] if (!state->is_initialized) {
    log_info("Initializing...");

    state->permanent.init(
        static_cast<u8*>(memory->permanent) + sizeof(EngineState), Permanent_Memory_Block_Size - sizeof(EngineState));

    state->camera.init(-90.0f, -27.0f, vec3(0.0f, 5.0f, 10.0f));

    state->pointer.x = 200;
    state->pointer.y = 200;
    state->input_mode = InputMode::Game;

    state->models.init(state->permanent, 6);
    state->models[0].id = 1;
    state->models[1].id = 2;
    state->models[2].id = 3;
    state->models[3].id = 4;
    state->models[4].id = 5;
    state->models[5].id = 6;
    import_model("assets/meshes/dungeon.fbx_Wall1/dungeon.fbx_Wall1.fbx", state->models[0], state->permanent);
    import_model("assets/meshes/dungeon.fbx_Wall2/dungeon.fbx_Wall2.fbx", state->models[1], state->permanent);
    import_model("assets/meshes/dungeon.fbx_Wall3/dungeon.fbx_Wall3.fbx", state->models[2], state->permanent);
    import_model("assets/meshes/dungeon.fbx_Wall4/dungeon.fbx_Wall4.fbx", state->models[3], state->permanent);
    import_model("assets/meshes/dungeon.fbx_Doorway/dungeon.fbx_Doorway.fbx", state->models[4], state->permanent);
    import_model("assets/meshes/dungeon.fbx_Floors/dungeon.fbx_Floors.fbx", state->models[5], state->permanent);

    state->floor.num_vertices = 4;
    state->floor.vertices = allocate<vec3>(state->permanent, 4);
    state->floor.vertices[0] = vec3(F32_MAX, 0, F32_MAX);
    state->floor.vertices[1] = vec3(-F32_MAX, 0, F32_MAX);
    state->floor.vertices[2] = vec3(-F32_MAX, 0, -F32_MAX);
    state->floor.vertices[3] = vec3(F32_MAX, 0, -F32_MAX);
    state->floor.transform = Transform();

    state->uniform_buffer_container.init(UniformBuffer::PerFrame, allocate<PerFrameData>(state->permanent), sizeof(PerFrameData));
    state->uniform_buffer_container.init(UniformBuffer::Light, allocate<LightData>(state->permanent), sizeof(LightData));
    state->uniform_buffer_container.init(UniformBuffer::Material, allocate<Material>(state->permanent), sizeof(Material));

    // region Compile shaders
    mesh_program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\phong.frag)");
    single_color_mesh_program.initialize(R"(.\assets\shaders\mesh.vert)", R"(.\assets\shaders\single_color.frag)");
    single_color_mesh_program.useProgram();
    single_color_mesh_program.set_uniform("color", vec4(0.7f, 0.1f, 0.2f, 0.0f));
    single_color_program.initialize(R"(.\assets\shaders\basic_2d.vert)", R"(.\assets\shaders\single_color.frag)");
    quad_program.initialize(R"(.\assets\shaders\quad.vert)", R"(.\assets\shaders\quad.frag)");
    font_program.initialize(R"(.\assets\shaders\font.vert)", R"(.\assets\shaders\font.frag)");
    grid_program.initialize(R"(.\assets\shaders\grid.vert)", R"(.\assets\shaders\grid.frag)");
    imgui_program.initialize(R"(.\assets\shaders\imgui.vert)", R"(.\assets\shaders\imgui.frag)");

    state->framebuffer.init(app_input->client_width, app_input->client_height);
    state->ms_framebuffer.init(app_input->client_width, app_input->client_height);

    state->entities.init(state->permanent, 100);

    assert(graphics_options != nullptr);
    read_from_file(graphics_options);

    // endregion

    for (auto& model : state->models) {
      for (auto& mesh : model.meshes) {
        mesh.vao.init();
        mesh.vao.bind();

        auto data_size = static_cast<GLsizeiptr>(sizeof(vec3) * mesh.num_vertices); // NOLINT
        assert(mesh.num_vertices == mesh.num_normals);

        // TODO: Should be packed into a single buffer
        // mesh.vao.add_buffer(data_size, 3, sizeof(vec3), 0, 0);
        mesh.vao.add_buffer(0, data_size, sizeof(vec3));
        mesh.vao.add_buffer_desc(0, 0, 3, 0, sizeof(vec3));
        mesh.vao.add_buffer(1, data_size, sizeof(vec3));
        mesh.vao.add_buffer_desc(1, 1, 3, 0, sizeof(vec3));
        mesh.vao.upload_buffer_desc();

        mesh.vao.upload_buffer_data(0, mesh.vertices, 0, data_size);
        mesh.vao.upload_buffer_data(1, mesh.normals, 0, data_size);
      }
    }

    float quad_verticies[] = {
      // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
      // clang-format off
      // positions     // texCoords
      -1.0f, 1.0f,     0.0f, 1.0f,  
      -1.0f, -1.0f,    0.0f, 0.0f, 
      1.0f, -1.0f,     1.0f, 0.0f,  
      -1.0f, 1.0f,     0.0f, 1.0f,  
      1.0f, -1.0f,     1.0f, 0.0f,  
      1.0f, 1.0f,      1.0f, 1.0f,
      // clang-format on
    };

    state->quad_vao.init();
    state->quad_vao.bind();
    state->quad_vao.add_buffer(0, sizeof(quad_verticies), 4 * sizeof(f32));
    state->quad_vao.add_buffer_desc(0, 0, 2, 0, 4 * sizeof(f32));
    state->quad_vao.add_buffer_desc(0, 1, 2, 2 * sizeof(f32), 4 * sizeof(f32));
    state->quad_vao.upload_buffer_desc();
    state->quad_vao.upload_buffer_data(0, quad_verticies, 0, sizeof(quad_verticies));

    state->camera.update_cursor(0, 0);

    auto cli_memory_arena = state->permanent.allocate_arena(MegaBytes(1));
    state->font = font_load("assets/fonts/ubuntu/Ubuntu-Regular.ttf", state->permanent);
    state->text_renderer.init(&font_program);
    state->cli.init(cli_memory_arena);

    im::initialize_imgui(state->font, &state->permanent);
    state->is_initialized = true;
  }

  // endregion

#if ENGINE_DEBUG
  state->permanent.check_integrity();
  asset_manager->update_if_changed();
#endif

  auto& time = state->time;
  time.dt_ms = app_input->dt_ms;
  time.dt = static_cast<f32>(time.dt_ms) * 0.001;
  time.t_ms = app_input->t_ms;
  time.num_frames++;
  time.second_counter += time.dt_ms;

  if (time.second_counter > 1000) {
    time.fps = time.num_frames;
    time.num_frames = 0;
    time.second_counter -= 1000;
  }

  const auto projection = perspective(45.0f, ratio, 0.1f, 100.0f);
  auto ortho_projection = create_ortho(0, app_input->client_width, 0, app_input->client_height, 0.0f, 100.0f);
  const auto inv_projection = inverse(projection);
  const auto view = state->camera.get_view();

  Pointer* pointer = &state->pointer;
  const MouseRaw* mouse = &app_input->input.mouse_raw;


  // Update GUI
  if (state->pointer_mode == PointerMode::NORMAL) {
    im::new_frame(pointer->x, pointer->y, app_input->input.mouse_raw.left.ended_down, &ortho_projection);
  } else {
    im::new_frame(-1, -1, false, &ortho_projection);
  }

  // Update CLI
  {
    // TODO: Need to rewrite CLI to use imgui
    if (app_input->input.oem_5.is_pressed_this_frame()) {
      state->is_cli_active = state->cli.toggle();
    }
    state->cli.handle_input(&app_input->input);
    state->cli.update(app_input->client_width, app_input->client_height, time.dt);
  }

  {
    im::window_begin(1, "My window", app_input->client_width - 200, app_input->client_height - 100);
    if (im::button(GEN_GUI_ID, "Gate")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[4], .transform = Transform() };
      state->entities.push(entity);
    }

    if (im::button(GEN_GUI_ID, "Wall 1")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[0], .transform = Transform() };
      state->entities.push(entity);
    }

    if (im::button(GEN_GUI_ID, "Wall 2")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[1], .transform = Transform() };
      state->entities.push(entity);
    }

    if (im::button(GEN_GUI_ID, "Wall 3")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[2], .transform = Transform() };
      state->entities.push(entity);
    }

    if (im::button(GEN_GUI_ID, "Wall 4")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[3], .transform = Transform() };
      state->entities.push(entity);
    }

    if (im::button(GEN_GUI_ID, "Floor")) {
      auto* last_entity = state->entities.last();
      auto new_id = last_entity ? last_entity->id + 1 : 1;
      auto entity = Entity{ .id = new_id, .model = &state->models[5], .transform = Transform() };
      state->entities.push(entity);
    }
    im::window_end();

    auto debug_color = vec4(1.0, 1.0, 0, 1.0);
    char fps_text[50];
    sprintf(fps_text, "FPS: %d", time.fps);
    auto fps_text_dim = font_str_dim(fps_text, 0.3, *state->font);
    im::text(fps_text, app_input->client_width - fps_text_dim.x - 25, app_input->client_height - fps_text_dim.y - 25,
        debug_color, 0.3);

    if (im::get_state().hot_item != im::None_Hot_Items_id) {
      state->input_mode = InputMode::Gui;
      state->pointer_mode = PointerMode::NORMAL;
    } else {
      state->input_mode = InputMode::Game;
    }
  }

  if (state->pointer_mode == PointerMode::NORMAL || state->pointer_mode == PointerMode::GRAB) {
    state->pointer.update_pos(*mouse, app_input->client_width, app_input->client_height);
  } else {
    state->camera.update_cursor(static_cast<f32>(mouse->dx), static_cast<f32>(mouse->dy));
  }
  // Update Game
  if (state->input_mode == InputMode::Game) {
    if (state->pointer_mode != PointerMode::GRAB) {
      state->hot_entity = nullptr;
      for (auto& entity : state->entities) {
        for (auto& mesh : entity.model->meshes) {
          // TODO: Check which one is in front
          vec2 intersections;
          if (intersects(state->camera.get_position(), pointer->ray, mesh.get_bbox(), entity.transform, intersections)) {
            state->hot_entity = &entity;
          }
        }
      }
    }
    state->pointer.update_ray(view, inv_projection, app_input->client_width, app_input->client_height);

    switch (state->pointer_mode) {
    case PointerMode::NORMAL:
      if (mouse->left.is_pressed_this_frame()) {
        if (state->hot_entity == nullptr) {
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
    } break;
    }

    vec2 floor_intersections;
    if (intersects(state->camera.get_position(), pointer->ray, state->floor.get_bbox(), state->floor.transform, floor_intersections) &&
        state->pointer_mode == PointerMode::GRAB && state->hot_entity != nullptr) {
      vec3 location = (pointer->ray * floor_intersections.x) + state->camera.get_position();
      location.x = roundf(location.x * 10.0) / 10.0;
      location.z = roundf(location.z * 10.0) / 10.0;
      state->hot_entity->transform.position = location;
    }

    if (intersects(state->camera.get_position(), pointer->ray, state->floor.get_bbox(), state->floor.transform, floor_intersections)) {
      vec3 location = (pointer->ray * floor_intersections.x) + state->camera.get_position();
    }

    if (app_input->input.r.is_pressed_this_frame() && state->pointer_mode == PointerMode::GRAB) {
      auto t = state->hot_entity->transform;
      auto new_angle = getAngle(t.rotation) + PI / 2;
      state->hot_entity->transform.rotation = angle_axis(new_angle, vec3(0, 1, 0));
    }
  }

  const Light light = {
    .position_ws = vec3(0, 2.0f, 2.0f),
    .radius = 20.0f,
    .color = vec3(1.0, 1.0, 1.0),
  };

  //////////////////////////////////
  ///////       Render       ///////
  //////////////////////////////////

  if (graphics_options->anti_aliasing) {
    gl->bind_framebuffer(GL_FRAMEBUFFER, state->ms_framebuffer.fbo);
  } else {
    gl->bind_framebuffer(GL_FRAMEBUFFER, state->framebuffer.fbo);
  }
  // region Render setup
  gl->enable(GL_DEPTH_TEST);
  gl->clear_color(0.1f, 0.1f, 0.1f, 0.2f);
  gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl->viewport(0, 0, app_input->client_width, app_input->client_height);

  mesh_program.useProgram();
  auto* mvp_ubuf = state->uniform_buffer_container.get_location<PerFrameData>(UniformBuffer::PerFrame);
  mvp_ubuf->projection = projection;
  mvp_ubuf->view = view;
  auto* light_ubuf = state->uniform_buffer_container.get_location<LightData>(UniformBuffer::Light);
  for (auto& entity : state->entities) {
    if (state->hot_entity != nullptr && entity.id == state->hot_entity->id && state->pointer_mode != PointerMode::LOOK_AROUND) {
      continue;
    }
    const mat4 m = entity.transform.to_mat4();
    *light_ubuf = light.to_data(inverse(m), state->camera.get_position());
    mvp_ubuf->model = m;
    for (auto& mesh : entity.model->meshes) {
      mesh.vao.bind();
      auto* material = state->uniform_buffer_container.get_location<Material>(UniformBuffer::Material);
      *material = mesh.material;
      state->uniform_buffer_container.upload();
      gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }
  }

  if (state->hot_entity != nullptr && state->pointer_mode != PointerMode::LOOK_AROUND) {
    gl->use_program(0);
    mesh_program.useProgram();
    enable_stencil_test();

    const mat4 m = state->hot_entity->transform.to_mat4();
    *light_ubuf = light.to_data(inverse(m), state->camera.get_position());
    mvp_ubuf->model = m;
    state->uniform_buffer_container.upload();

    for (auto& mesh : state->hot_entity->model->meshes) {
      mesh.vao.bind();
      auto* material = state->uniform_buffer_container.get_location<Material>(UniformBuffer::Material);
      *material = mesh.material;
      state->uniform_buffer_container.upload();
      gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }

    // Outline
    enable_outline();
    single_color_mesh_program.useProgram();
    auto t = state->hot_entity->transform;
    t.scale.x = 1.1;
    t.scale.y = 1.1;
    t.scale.z = 1.1;

    mvp_ubuf->model = t.to_mat4();
    state->uniform_buffer_container.upload();
    for (auto& mesh : state->hot_entity->model->meshes) {

      mesh.vao.bind();
      gl->draw_arrays(GL_TRIANGLES, 0, mesh.num_vertices);
    }
    disable_stencil_test();
  }

  // endregion

  // endregion

  // region Draw grid
  {
    if (graphics_options->enable_grid) {
      GLVao vao{};
      vao.init();
      vao.bind();
      grid_program.useProgram();
      gl->enable(GL_DEPTH_TEST);
      gl->enable(GL_BLEND);
      gl->blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      state->uniform_buffer_container.upload();
      gl->draw_arrays_instanced_base_instance(GL_TRIANGLES, 0, 6, 1, 0);
      gl->disable(GL_BLEND);
      vao.destroy();
    }
  }
  // endregion

  // region RenderGUI
  {
    gl->enable(GL_BLEND);
    gl->disable(GL_DEPTH_TEST);
    gl->blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl->bind_texture(GL_TEXTURE_2D, state->font->texture_atlas);
    single_color_program.useProgram();

    imgui_program.useProgram();
    imgui_program.set_uniform("projection", ortho_projection);

    auto layers = im::get_render_layers();
    GLVao vao;
    vao.init();
    vao.bind();

    // TODO: Remove 1024, im::gui needs to decide this
    // Allocate buffers
    vao.set_element_buffer(sizeof(i32) * 4* 1024);
    auto total_size = 4*1024 * sizeof(im::DrawVert);
    auto stride = sizeof(im::DrawVert);
    vao.add_buffer(0, total_size, stride);
    vao.add_buffer_desc(0, 0, 2, offsetof(im::DrawVert, position), stride);
    vao.add_buffer_desc(0, 1, 2, offsetof(im::DrawVert, uv), stride);
    vao.add_buffer_desc(0, 2, 4, offsetof(im::DrawVert, color), stride);
    vao.upload_buffer_desc();

    auto i = 0;
    for (auto& layer : layers) {
      /*if (i > 3) {*/
      /*  printf("Vertices:\n");*/
      /*  layer.vertices.data()[0].print();*/
      /*}*/
      vao.upload_element_buffer_data(layer.indices.data(), sizeof(i32) * layer.indices.size());
      vao.upload_buffer_data(0, layer.vertices.data(), 0, sizeof(im::DrawVert) * layer.vertices.size());
      gl->draw_elements(GL_TRIANGLES, layer.indices.size(), GL_UNSIGNED_INT, 0);
      i++;
    }

    vao.destroy();
    gl->enable(GL_DEPTH_TEST);
    gl->disable(GL_BLEND);
  }

  // region Draw pointer
  if (state->pointer_mode != PointerMode::LOOK_AROUND) {
    single_color_program.useProgram();
    single_color_program.set_uniform("color", vec4(0.7f, 0.7f, 0.7f, 0.7f));
    single_color_program.set_uniform("projection", ortho_projection);
    // TODO Fix this, worst implementation ever. Move it to GUI library
    f32 x = state->pointer.x;
    f32 y = state->pointer.y;
    f32 cursor_vertices[6] = {
      x, y,                                                         //
      x + new_x(-10.0f, -20.0f, 45.0f), y + new_y(-10, -20, 45.0f), //
      x + new_x(10.0f, -20.0f, 45.0f), y + new_y(10, -20, 45.0f),   //
    };
    GLVao cursor_vao{};
    cursor_vao.init();
    cursor_vao.bind();
    cursor_vao.add_buffer(0, sizeof(cursor_vertices), sizeof(vec2));
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
    // TODO: Is this needed?
    gl->clear_color(1.0f, 1.0f, 1.0f, 1.0f);
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

void load(GLFunctions* in_gl, Platform* in_platform, EngineMemory* in_memory) {
  load_gl(in_gl);
  platform = in_platform;

  assert(sizeof(EngineState) < Permanent_Memory_Block_Size);
  auto* state = (EngineState*)in_memory->permanent;
  state->transient.init(in_memory->transient, Transient_Memory_Block_Size);
  set_transient_arena(&state->transient);

  assert(sizeof(AssetManager) <= Assets_Memory_Block_Size);
  asset_manager_set_memory(in_memory->asset);

  graphics_options = &state->graphics_options;
}
