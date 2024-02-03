#pragma once

#include "cli_app.h"
#include "../engine.h"

auto inline handle_graphics(Array<FStr> &args, LinkedListBuffer &buf) -> void {
    const char* help_message = "graphics anti-aliasing on|off";
    if (args.size() != 2) {
        buf.add(help_message);
        return;
    }

    if (args[0] == "antialias") {
        if (args[1] == "on") {
            graphics_options->anti_aliasing = true;
        }
        else if (args[1] == "off") {
            graphics_options->anti_aliasing = false;
        }
        else {
            buf.add(help_message);
        }
        return;
    }
    else {
        buf.add(help_message);
    }
}

auto inline register_graphics(FList<CliApp> &apps, MemoryArena &arena) -> void {
    CliApp echo {
            .name = FStr::create("graphics", arena),
            .handle = &handle_graphics
    };
    apps.push(echo);
}
