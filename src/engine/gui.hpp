#pragma once

#include "math/vec2.h"
#include "math/vec4.h"
#include "memory_arena.h"
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
  vec4 color;
};

struct UIState {
  i32 mouse_x;
  i32 mouse_y;
  bool mouse_down;

  i32 hot_item;
  i32 active_item;
};

struct RenderData {
  DrawVert vertices[1024];
  i32 num_vertices;
  i32 indices[1024];
  i32 num_indices;
};

auto initialize(const char* font_path, MemoryArena* permanent);

auto new_frame(i32 mouse_x, i32 mouse_y, bool mouse_down) -> void;
auto get_render_data() -> RenderData*;

auto button(i32 id, i32 x, i32 y, const char* text) -> void;
} // namespace im
