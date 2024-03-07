#include "gui.hpp"
#include "platform/types.h"
#include "text_renderer.h"

namespace im {

RenderData render_data;
UIState state;
TextRenderer* _text_renderer;
Font* _font;
mat4* _ortho;

auto initialize_imgui(Font* font, TextRenderer* text_renderer) -> void {
  _text_renderer = text_renderer;
  _font = font;
}

auto new_frame(i32 mouse_x, i32 mouse_y, bool mouse_down, mat4* ortho) -> void {
  state.mouse_x = mouse_x;
  state.mouse_y = mouse_y;
  state.mouse_down = mouse_down;
  render_data.num_vertices = 0;
  render_data.num_indices = 0;
  _ortho = ortho;
}

auto get_render_data() -> RenderData* {
  return &render_data;
}

const vec4 background_color = vec4(0.133, 0.239, 0.365, 0);
const vec4 active_color = vec4(0.233, 0.339, 0.465, 0);
const vec4 hot_color = vec4(0.033, 0.139, 0.265, 0);

auto button(i32 id, i32 x, i32 y, const char* text) -> void {
  const i32 button_min_width = 150;
  const i32 button_height = 50;
  vec2 start = vec2(x, y);
  vec2 end = vec2(x + button_min_width, y + button_height);

  auto color = background_color;
  if (state.mouse_x >= start.x && state.mouse_x < end.x && state.mouse_y >= start.y && state.mouse_y < end.y) {
    state.hot_item = id;

    if (state.mouse_down) {
      state.active_item = id;
    }
  } else if (state.hot_item == id) {
    state.hot_item = -1;
  }

  if (!state.mouse_down && state.active_item == id) {
    state.active_item = -1;
  }

  if (state.active_item == id) {
    color = active_color;
  } else if (state.hot_item == id) {
    color = hot_color;
  }

  render_data.vertices[0] = { .position = start, .color = color };
  render_data.vertices[1] = { .position = vec2(start.x, end.y), .color = color };
  render_data.vertices[2] = { .position = vec2(end.x, start.y), .color = color };
  render_data.vertices[3] = { .position = end, .color = color };
  render_data.num_vertices = 4;

  render_data.indices[0] = 0;
  render_data.indices[1] = 1;
  render_data.indices[2] = 2;

  render_data.indices[3] = 1;
  render_data.indices[4] = 3;
  render_data.indices[5] = 2;
  render_data.num_indices = 6;

  _text_renderer->render(text, *_font, x, y, 1.0, *_ortho);
  // text_renderer.render(text, i32 length, f32 x, f32 y, f32 scale, const mat4 &ortho_projection)
}
} // namespace im
