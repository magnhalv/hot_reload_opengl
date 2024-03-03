#ifndef HOT_RELOAD_OPENGL_CLI_H
#define HOT_RELOAD_OPENGL_CLI_H

#include <platform/types.h>

#include "../gl/gl_shader.h"
#include "../gl/gl_vao.h"
#include "../linked_list_buffer.h"
#include "../list.h"
#include "../text_renderer.h"

#include "cli_app.h"

struct CliSizes {
  static constexpr f32 padding_x = 5;
  static constexpr f32 line_height = 30;
  static constexpr f32 line_margin = 5;
  f32 message_area_height;
  f32 max_line_width;
  i32 max_num_lines;

  f32 width;
  f32 height;

  f32 scale;

  auto update(f32 _width, f32 _height) {
    max_line_width = _width - 2 * padding_x;
    // Margin from command line
    width = _width;
    height = _height;
    message_area_height = height - line_height - line_margin * 2;
    max_num_lines = static_cast<i32>(ceilf(message_area_height / line_height));
    scale = 0.4;
  }
};

struct Cli {
  static const i32 Max_Height = 200;
  static constexpr f32 Duration = 0.2f;

  GLVao _vao;
  GLShaderProgram* _single_color_program;
  auto handle_input(UserInput* input) -> void;
  auto init(GLShaderProgram* single_color, GLShaderProgram* font, MemoryArena* arena) -> void;
  auto update(f32 client_width, f32 client_height, f32 dt) -> void;
  /// Returns true if toggle enables cli
  auto toggle(f32 client_height) -> bool;
  auto render_background(f32 client_width, f32 client_height) -> void;
  auto render_text(f32 client_width, f32 client_height) -> void;

  auto execute_command(FStr& command) -> void;

  f32 t;
  f32 _current_y;
  f32 _progress;
  f32 _direction;
  f32 _target_y;
  f32 _current_height;

  MemoryArena* _arena;

  FList<char> _command_buffer;
  LinkedListBuffer _response_buffer;

  FList<CliApp> _apps;

  CliSizes _sizes;
  TextRenderer _renderer;
};

#endif // HOT_RELOAD_OPENGL_CLI_H
