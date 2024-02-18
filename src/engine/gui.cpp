#include "gui.hpp"

namespace im {

RenderData render_data;
UIState state;

struct DrawVert {
  vec2 postion;
  u32 color;
};

auto new_frame() -> void {
  render_data.num_vertices = 0;
  render_data.num_indices = 0;
}

auto get_render_data() -> RenderData* {
  return &render_data;
}

auto button(i32 x, i32 y) -> void {
  vec2 start = vec2(x, y);
  vec2 end = vec2(x + 100, y + 150);

  render_data.vertices[0] = start;
  render_data.vertices[1] = vec2(start.x, end.y);
  render_data.vertices[2] = vec2(end.x, start.y);
  render_data.vertices[3] = end;
  render_data.num_vertices = 4;

  render_data.indices[0] = 0;
  render_data.indices[1] = 1;
  render_data.indices[2] = 2;

  render_data.indices[3] = 1;
  render_data.indices[4] = 3;
  render_data.indices[5] = 2;
  render_data.num_indices = 6;
}
} // namespace im
