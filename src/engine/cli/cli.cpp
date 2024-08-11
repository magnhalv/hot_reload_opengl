#include "cli.h"
#include <math/vec2.h>

#include "../gui.hpp"
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
  if (m_active == -1) {
    return;
  }

  if (!m_command_buffer.is_full()) {
    for (u8 i = 0; i < 26; i++) {
      auto button = input->buttons[i];

      if (button.is_pressed_this_frame()) {
        char character[2];
        character[0] = i + 97;
        character[1] = '\0';
        m_command_buffer.push(character);
      }
    }

    if (input->space.is_pressed_this_frame()) {
      m_command_buffer.push(" ");
    } 
    else if (input->enter.is_pressed_this_frame()) {
      FStr command = FStr::create(m_command_buffer.data(), *m_permanent_arena);
      execute_command(command);
      if (m_command_buffer.len() > num_start_characters) {
        m_command_buffer.pop(m_command_buffer.len() - num_start_characters);
      }
    }   
  }

  if (input->back.is_pressed_this_frame()) {
    if (m_command_buffer.len() > num_start_characters) {
      m_command_buffer.pop();
    }
  }
}

auto Cli::init(MemoryArena* arena) -> void {
  m_active = 1;


  m_permanent_arena = arena;
  m_command_buffer = GStr::create("> ", 128, *arena);

  m_response_buffer.init(128 * 512, *arena);

  // region Init commands
  _apps.init(*arena, 5);
  register_echo(_apps, *arena);
  register_graphics(_apps, *arena);

  m_background = vec4(0.0f, 0.165f, 0.22f, 0.9f);
}

auto Cli::toggle() -> bool {

  m_active = -m_active;
  return m_active == 1;
}

auto Cli::update(i32 client_width, i32 client_height, f32 dt) -> void {
#if ENGINE_DEBUG
  m_permanent_arena->check_integrity();
#endif

  _sizes.line_height = im::get_font_max_height() + _sizes.line_margin;

  auto delta_progress = dt / Duration;
  _progress += delta_progress * m_active;

  if (_progress > 1.0) {
    _progress = 1.0;
  }
  else if (_progress < 0) {
    _progress = 0;
  }

  vec2 p0 = { 0, 0 };
  vec2 p1 = { 0.60, 0 };
  vec2 p2 = { 0.60, 1 };
  vec2 p3 = { 1, 1 };

  auto bezier = cubic_bezier(p0, p1, p2, p3, _progress).x;
  m_current_y = client_height - (client_height / 2.0f) * bezier;

  _sizes.update(client_width, m_current_y);

  im::set_to_top_layer();
  im::rectangle(GEN_GUI_ID, 0, m_current_y, client_width, client_height, m_background);

  const f32 x_inner_margin = 20;
  const f32 y_inner_margin = 5;
  const f32 y_command_start = m_current_y + y_inner_margin;

  auto lines_in_buffer = m_response_buffer.list.size();
  auto line_index_start = lines_in_buffer > _sizes.max_num_lines ? (lines_in_buffer - _sizes.max_num_lines) : 0;
  vec4 color{ 1.0, 1.0, 1.0, 1.0 };
  if (_sizes.max_num_lines > 0) {
    auto line_nr = 1;
    for (auto i = line_index_start; i < lines_in_buffer; i++) {
      auto& line = m_response_buffer.list[i];
      auto y_line_start = y_command_start + _sizes.line_height * line_nr;
      im::text(line.data(), x_inner_margin, y_line_start, color, _sizes.scale);
      line_nr++;
    }
  }
  im::text(m_command_buffer.data(), x_inner_margin, y_command_start, color, _sizes.scale);
  im::reset_layer();
}

auto Cli::execute_command(FStr& command) -> void {
  m_response_buffer.add(command);
  auto split_command = split(command, ' ', *g_transient);
  for (auto app : _apps) {
    if (split_command[1] == app.name) {
      auto command_wo_app_name = split_command.size() > 2 ? span(split_command, 2) : Array<FStr>();
      app.handle(command_wo_app_name, m_response_buffer);
      return;
    }
  }

  m_response_buffer.add("  Command not found");
}
