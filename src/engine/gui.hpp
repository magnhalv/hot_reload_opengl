#include "math/vec2.h"

struct UIState {
  i32 mouse_x;
  i32 mouse_y;
  i32 mouse_down;

  i32 hot_item;
  i32 active_item;
};

struct RenderData {
  vec2 vertices[1024];
  i32 num_vertices;
  i32 indices[1024];
  i32 num_indices;
};
namespace im {

auto new_frame() -> void;
auto get_render_data() -> RenderData*;

auto button(i32 x, i32 y) -> void;
} // namespace im
