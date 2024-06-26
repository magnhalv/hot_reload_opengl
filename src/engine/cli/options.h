#pragma once

#include "../engine.h"
#include "../options.hpp"
#include "cli_app.h"

auto inline handle_options(Array<FStr>& args, LinkedListBuffer& buf) -> void {
  const char* help_message = "options [antialias, grid] on|off";
  if (args.size() != 2) {
    buf.add(help_message);
    return;
  }

  if (args[0] == "antialias") {
    if (args[1] == "on") {
      graphics_options->anti_aliasing = true;
    } else if (args[1] == "off") {
      graphics_options->anti_aliasing = false;
    } else {
      buf.add(help_message);
    }
  } else if (args[0] == "grid") {
    if (args[1] == "on") {
      graphics_options->enable_grid = true;
    } else if (args[1] == "off") {
      graphics_options->enable_grid = false;
    } else {
      buf.add(help_message);
    }
  } else {
    buf.add(help_message);
  }

  save_to_file(graphics_options);
}

auto inline register_graphics(List<CliApp>& apps, MemoryArena& arena) -> void {
  CliApp echo{ .name = FStr::create("options", arena), .handle = &handle_options };
  apps.push(echo);
}
