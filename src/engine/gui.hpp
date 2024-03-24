#pragma once

#include "list.h"
#include "math/vec2.h"
#include "math/vec4.h"
#include "text_renderer.h"

// Source: https://solhsa.com/imgui/ch04.html
// If you're going to render widgets to the same
// UI from different source files, you can avoid
// ID collisions by defining IMGUI_SRC_ID before
// this define block:
#ifdef IMGUI_SRC_ID
#define GEN_GUI_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_GUI_ID (__LINE__)
#endif

namespace im {

struct DrawVert {
  vec2 position;
  vec2 uv;
  vec4 color;
};

struct UIState {
  i32 mouse_x;
  i32 mouse_y;
  bool mouse_down;

  i32 hot_item;
  i32 active_item;
};

struct RenderLayer {
  FList<DrawVert> vertices;
  FList<i32> indices;
};

auto initialize_imgui(Font* font, MemoryArena* permanent) -> void;

auto new_frame(i32 mouse_x, i32 mouse_y, bool mouse_down, mat4* ortho) -> void;
auto end_frame() -> void;
auto get_render_layers() -> FList<RenderLayer>;

auto button(i32 id, const char* text, i32 x = 0, i32 y = 0) -> bool;
auto window_begin(i32 id, const char* title, i32 x, i32 y) -> void;
auto window_end() -> void;
auto text(const char* text, i32 x, i32 y, vec4& color, f32 scale) -> void;
} // namespace im
