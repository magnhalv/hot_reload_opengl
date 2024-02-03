#pragma once

#include "../memory_arena.h"
#include "../fixed_string.h"
#include "../linked_list.h"
#include "../list.h"
#include "../growable_list.h"
#include "../linked_list_buffer.h"

typedef void (*cli_command_handle)(Array<FStr> &args, LinkedListBuffer &buf);

struct CliApp {
    FStr name;
    cli_command_handle handle;
};
