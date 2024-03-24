#include "gui.hpp"
#include "memory_arena.h"
#include "platform/types.h"
#include "text_renderer.h"

namespace im {

UIState state;
Font* _font;
mat4* _ortho;
MemoryArena* gui_permanent;
MemoryArena* gui_transient;

FList<RenderLayer> layers;
i32 current_layer_idx = 0;

struct Window {
  i32 id;
  vec2 position;
  vec2 next_draw_point;
  vec2 size;
};

Window active_window;

const f32 ui_scale = 0.6f;

auto initialize_imgui(Font* font, MemoryArena* permanent) -> void {
  _font = font;
  gui_permanent = permanent->allocate_arena(MegaBytes(1));
  gui_transient = permanent->allocate_arena(MegaBytes(1));

  layers.init(*gui_permanent, 5);
  current_layer_idx = 0;
}

auto new_frame(i32 mouse_x, i32 mouse_y, bool mouse_down, mat4* ortho) -> void {
  state.mouse_x = mouse_x;
  state.mouse_y = mouse_y;
  state.mouse_down = mouse_down;

  layers.empty();
  current_layer_idx = 0;
  auto first_layer = layers.push();
  first_layer->vertices.init(*gui_transient, 1024);
  first_layer->indices.init(*gui_transient, 1024);

  active_window.id = 0;

  gui_transient->clear();
  _ortho = ortho;
}

auto end_frame() -> void {
}

auto get_render_layers() -> FList<RenderLayer> {
  return layers;
}

const vec4 background_color = vec4(0.133, 0.239, 0.365, 1.0);
const vec4 active_color = vec4(0.233, 0.339, 0.465, 1.0);
const vec4 hot_color = vec4(0.033, 0.139, 0.265, 1.0);

auto render_text(const char* text, const Font& font, f32 x, f32 y, f32 scale, vec4 color, const mat4& ortho_projection) -> void {
  auto length = strlen(text);
  auto& characters = font.characters;
  auto* layer = &layers[current_layer_idx];
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

    auto index = layer->vertices.size();
    layer->vertices.push({ .position = vec2(x_pos, y_pos + h), .uv = vec2(ch.uv_start.x, ch.uv_start.y), .color = color }); // 1
    layer->vertices.push({ .position = vec2(x_pos, y_pos), .uv = vec2(ch.uv_start.x, ch.uv_end.y), .color = color }); // 2
    layer->vertices.push({ .position = vec2(x_pos + w, y_pos), .uv = vec2(ch.uv_end.x, ch.uv_end.y), .color = color }); // 3
    layer->vertices.push({ .position = vec2(x_pos + w, y_pos + h), .uv = vec2(ch.uv_end.x, ch.uv_start.y), .color = color }); // 4

    layer->indices.push(index);
    layer->indices.push(index + 1);
    layer->indices.push(index + 2);

    layer->indices.push(index);
    layer->indices.push(index + 2);
    layer->indices.push(index + 3);
    // render glyph texture over quad
    // update content of VBO memory

    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
  }
}

auto text(const char* text, i32 x, i32 y, vec4& color, f32 scale) -> void {
  render_text(text, *_font, x, y, scale, color, *_ortho);
}

auto draw_rectangle(f32 x, f32 y, f32 width, f32 height, vec4 color) {
  auto uv = vec2(0.0, 0.0);
  vec2 start = vec2(x, y);
  vec2 end = vec2(x + width, y + height);
  auto* current_layer = &layers[current_layer_idx];
  auto first_index = current_layer->vertices.size();
  current_layer->vertices.push({ .position = start, .uv = uv, .color = color });
  current_layer->vertices.push({ .position = vec2(start.x, end.y), .uv = uv, .color = color });
  current_layer->vertices.push({ .position = vec2(end.x, start.y), .uv = uv, .color = color });
  current_layer->vertices.push({ .position = end, .uv = uv, .color = color });

  current_layer->indices.push(first_index);
  current_layer->indices.push(first_index + 1);
  current_layer->indices.push(first_index + 2);

  current_layer->indices.push(first_index + 1);
  current_layer->indices.push(first_index + 3);
  current_layer->indices.push(first_index + 2);
}

auto button(i32 id, const char* text, i32 x, i32 y) -> bool {
  vec2 pos = vec2(x, y);
  if (active_window.id != 0) {
    pos = active_window.next_draw_point;
  }

  const i32 padding = 15;
  const i32 margin = 15;

  auto text_dim = font_str_dim(text, ui_scale, *_font);
  vec2 size = vec2(text_dim.x + padding * 2, text_dim.y + padding * 2);
  vec2 start = pos;
  vec2 end = pos + size;

  printf("State mouse: %d %d \n", state.mouse_x, state.mouse_y);
  auto color = background_color;
  if (state.mouse_x >= start.x && state.mouse_x < end.x && state.mouse_y >= start.y && state.mouse_y < end.y) {
    state.hot_item = id;
    printf("Hot item: %d\n", state.hot_item);
    printf("Start: %f %f \n", start.x, start.y);
    printf("End: %f %f \n", end.x, end.y);

    if (state.mouse_down) {
      state.active_item = id;
    }
  } else if (state.hot_item == id) {
    state.hot_item = -1;
  }

  bool was_clicked = !state.mouse_down && state.active_item == id;
  if (was_clicked) {
    state.active_item = -1;
  }

  if (state.active_item == id) {
    color = active_color;
  } else if (state.hot_item == id) {
    color = hot_color;
  }

  draw_rectangle(start.x, start.y, size.x, size.y, color);

  active_window.next_draw_point.y += size.y + margin;

  vec4 text_color = vec4(0.7, 0.7, 0.7, 1.0);
  render_text(text, *_font, start.x + padding, start.y + padding, ui_scale, text_color, *_ortho);

  return was_clicked;
  // text_renderer.render(text, i32 length, f32 x, f32 y, f32 scale, const mat4 &ortho_projection)
}

auto window_begin(i32 id, const char* title, i32 x, i32 y) -> void {
  const i32 padding_x = 15;
  const i32 padding_y = 15;
  active_window.id = id;
  active_window.position.x = x;
  active_window.position.y = y;
  active_window.next_draw_point = vec2(x + padding_x, y + padding_y);

  auto new_layer = layers.push();
  new_layer->vertices.init(*gui_transient, 1024);
  new_layer->indices.init(*gui_transient, 1024);
  current_layer_idx++;
  assert(current_layer_idx < 5);
}

auto window_end() -> void {
  assert(current_layer_idx > 0);
  current_layer_idx--;
  active_window.id = 0;
}
} // namespace im
