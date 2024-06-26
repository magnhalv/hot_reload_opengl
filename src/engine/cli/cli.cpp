#include "cli.h"
#include <math/vec2.h>

#include "echo.h"
#include "options.h"

const size_t RawBufferSize = KiloBytes(512);
const size_t num_start_characters = 2;

auto cubic_bezier(vec2 p0, vec2 p1, vec2 p2, vec2 p3, f32 t) -> vec2 {
  const f32 b = 1 - t;
  return {
    powf(b, 3) * p0.x + 3 * powf(b, 2) * t * p1.x + 3 * b * powf(t, 2) * p2.x + powf(t, 3) * p3.x,
    powf(b, 3) * p0.y + 3 * powf(b, 2) * t * p1.y + 3 * b * powf(t, 2) * p2.y + powf(t, 3) * p3.y,
  };
}

auto Cli::handle_input(UserInput* input) -> void {
  if (m_active != -1) {
    return;
  }

  if (!_command_buffer.is_full()) {
    for (u8 i = 0; i < 26; i++) {
      auto button = input->buttons[i];

      if (button.is_pressed_this_frame()) {
        char character[2];
        character[0] = i + 97;
        character[1] = '\0';
        _command_buffer.push(character);
      }
    }

    if (input->space.is_pressed_this_frame()) {
      _command_buffer.push(" ");
    } else if (input->enter.is_pressed_this_frame()) {
      FStr command = FStr::create(_command_buffer.data(), *_arena);
      execute_command(command);
      if (_command_buffer.len() > num_start_characters) {
        _command_buffer.pop(_command_buffer.len() - num_start_characters);
      }
    } else if (input->back.is_pressed_this_frame() && _command_buffer.len() > 2) {
      _command_buffer.pop();
    }
  }

  if (input->back.is_pressed_this_frame()) {
    if (_command_buffer.len() > num_start_characters) {
      _command_buffer.pop();
    }
  }
}

auto Cli::init(GLShaderProgram* single_color, TextRenderer* text_renderer, Font* font, MemoryArena* arena) -> void {
  _vao.init();
  _vao.bind();
  _vao.add_buffer(0, 2 * 6 * sizeof(f32), 2 * sizeof(f32));
  _vao.add_buffer_desc(0, 0, 2, 0, 2 * sizeof(f32));
  _vao.upload_buffer_desc();

  _single_color_program = single_color;
  m_active = -1;

  _arena = arena;
  _command_buffer = GStr::create("> ", 128, *arena);

  _response_buffer.init(128 * 512, *arena);

  // region Init commands
  _apps.init(*arena, 5);
  register_echo(_apps, *arena);
  register_graphics(_apps, *arena);

  assert(text_renderer != nullptr);
  _text_renderer = text_renderer;
  _font = font;
}

auto Cli::toggle(f32 client_height) -> bool {
  _target_y = client_height / 2;

  m_active = -m_active;
  return m_active == 1;
}

auto Cli::update(f32 client_width, f32 client_height, f32 dt) -> void {
#if ENGINE_DEBUG
  _arena->check_integrity();
#endif

  auto delta_progress = dt / Duration;
  _progress += delta_progress * m_active;

  if (_progress > 1.0) {
    _progress = 1.0;
  }

  if (_progress < 0) {
    _progress = 0;
  }

  vec2 p0 = { 0, 0 };
  vec2 p1 = { 0.60, 0 };
  vec2 p2 = { 0.60, 1 };
  vec2 p3 = { 1, 1 };

  auto bezier = cubic_bezier(p0, p1, p2, p3, _progress).x;
  _current_y = client_height - _target_y * bezier;
  _current_height = client_height - _current_y;

  _sizes.update(client_width, _current_height);
}

auto Cli::render_background(f32 client_width, f32 client_height) -> void {
  _vao.bind();
  gl->enable(GL_BLEND);
  gl->blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  _single_color_program->useProgram();
  _single_color_program->set_uniform("color", vec4(0.0f, 0.165f, 0.22f, 0.9f));

  f32 cursor_vertices[] = {
    0.0f,
    _current_y,
    client_width,
    client_height,
    0.0f,
    client_height,

    0.0f,
    _current_y,
    client_width,
    client_height,
    client_width,
    _current_y,
  };
  _vao.upload_buffer_data(0, cursor_vertices, 0, sizeof(cursor_vertices));
  gl->draw_arrays(GL_TRIANGLES, 0, 6);
  gl->disable(GL_BLEND);
}

auto Cli::render_text(f32 client_width, f32 client_height) -> void {
  const f32 padding = 20;
  auto ortho = create_ortho(0, client_width, 0, client_height, 0.0f, 100.0f);
  f32 line_x = padding;
  f32 line_y = client_height - (client_height - _current_y) + padding;
  _text_renderer->render(_command_buffer.data(), *_font, line_x, line_y, _sizes.scale, ortho);

  auto line_nr = 1;
  auto offset = _target_y - _current_y;
  auto resp_size = _response_buffer.list.size();
  auto line_index_start = resp_size > _sizes.max_num_lines ? (resp_size - _sizes.max_num_lines) : 0;
  if (_sizes.max_num_lines > 0) {
    for (auto i = line_index_start; i < resp_size; i++) {
      auto& line = _response_buffer.list[i];
      _text_renderer->render(line.data(), *_font, line_x, client_height - offset - _sizes.line_height * line_nr, _sizes.scale, ortho);
      line_nr++;
    }
  }
}

auto Cli::execute_command(FStr& command) -> void {
  _response_buffer.add(command);
  auto split_command = split(command, ' ', *g_transient);
  for (auto app : _apps) {
    if (split_command[1] == app.name) {
      auto command_wo_app_name = split_command.size() > 2 ? span(split_command, 2) : Array<FStr>();
      app.handle(command_wo_app_name, _response_buffer);
      return;
    }
  }

  _response_buffer.add("  Command not found");
}
