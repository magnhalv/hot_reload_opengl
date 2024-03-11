#include "gui.hpp"
#include "math/math.h"
#include "memory_arena.h"
#include "platform/types.h"
#include "text_renderer.h"

namespace im {

RenderData render_data;
UIState state;
Font* _font;
mat4* _ortho;
MemoryArena* gui_arena;

auto initialize_imgui(Font* font, MemoryArena* permanent) -> void {
  _font = font;
  gui_arena = permanent->allocate_arena(MegaBytes(2));
  render_data.vertices.init(*gui_arena, 1024);
  render_data.indices.init(*gui_arena, 1024);
}

auto new_frame(i32 mouse_x, i32 mouse_y, bool mouse_down, mat4* ortho) -> void {
  state.mouse_x = mouse_x;
  state.mouse_y = mouse_y;
  state.mouse_down = mouse_down;
  render_data.vertices.empty();
  render_data.indices.empty();
  _ortho = ortho;
}

auto get_render_data() -> RenderData* {
  return &render_data;
}

const vec4 background_color = vec4(0.133, 0.239, 0.365, 1.0);
const vec4 active_color = vec4(0.233, 0.339, 0.465, 1.0);
const vec4 hot_color = vec4(0.033, 0.139, 0.265, 1.0);

auto render_text(const char* text, const Font& font, f32 x, f32 y, f32 scale, const mat4& ortho_projection) -> void {
  auto length = strlen(text);
  auto& characters = font.characters;
  for (auto i = 0; i < length; i++) {
    char c = text[i];
    if (c == '\0') {
      continue;
    }
    assert(c > 0 && c < characters.size());
    Character ch = characters[c];

    f32 x_pos = x + ch.bearing.x * scale;
    f32 y_pos = y - (ch.size.y - ch.bearing.y) * scale;

    f32 w = ch.size.x * scale;
    f32 h = ch.size.y * scale;

    vec4 color = vec4(0.7, 0.7, 0.7, 1.0);
    render_data.vertices.push({ .position = vec2(x_pos, y_pos + h), .uv = vec2(ch.uv_start.x, ch.uv_start.y), .color = color }); // 1
    render_data.vertices.push({ .position = vec2(x_pos, y_pos), .uv = vec2(ch.uv_start.x, ch.uv_end.y), .color = color }); // 2
    render_data.vertices.push({ .position = vec2(x_pos + w, y_pos), .uv = vec2(ch.uv_end.x, ch.uv_end.y), .color = color }); // 3
    render_data.vertices.push({ .position = vec2(x_pos + w, y_pos + h), .uv = vec2(ch.uv_end.x, ch.uv_start.y), .color = color }); // 4

    auto index = render_data.vertices.size() - 4;
    render_data.indices.push(index);
    render_data.indices.push(index + 1);
    render_data.indices.push(index + 2);

    render_data.indices.push(index);
    render_data.indices.push(index + 2);
    render_data.indices.push(index + 3);
    // render glyph texture over quad
    // update content of VBO memory

    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
  }
}

auto button(i32 id, i32 x, i32 y, const char* text) -> void {
  i32 button_width = 150;
  i32 button_height = 50;
  const i32 padding = 15;

  auto text_dim = font_str_dim(text, 0.7, *_font);
  button_width = text_dim.x + padding * 2;
  button_height = text_dim.y + padding * 2;

  vec2 start = vec2(x, y);
  vec2 end = vec2(x + button_width, y + button_height);

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

  auto uv = vec2(0.0, 0.0);
  render_data.vertices.push({ .position = start, .uv = uv, .color = color });
  render_data.vertices.push({ .position = vec2(start.x, end.y), .uv = uv, .color = color });
  render_data.vertices.push({ .position = vec2(end.x, start.y), .uv = uv, .color = color });
  render_data.vertices.push({ .position = end, .uv = uv, .color = color });

  render_data.indices.push(0);
  render_data.indices.push(1);
  render_data.indices.push(2);

  render_data.indices.push(1);
  render_data.indices.push(3);
  render_data.indices.push(2);

  render_text(text, *_font, x + padding, y + padding, 0.7, *_ortho);

  // text_renderer.render(text, i32 length, f32 x, f32 y, f32 scale, const mat4 &ortho_projection)
}
} // namespace im
