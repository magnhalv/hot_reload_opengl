#pragma once

#include "cli_app.h"

auto inline handle_echo(Array<FStr> &args, LinkedListBuffer &buf) -> void {
    if (args.size() != 1) {
        buf.add("Echo only accepts a single argument.");
        return;
    }
    else {
        buf.add(args[0].data());
    }
}

auto inline register_echo(FList<CliApp> &apps, MemoryArena &arena) -> void {
    CliApp echo {
        .name = FStr::create("echo", arena),
        .handle = &handle_echo
    };
    apps.push(echo);
}
