#pragma once

#include "memory_arena.h"
#include "fixed_string.h"
#include "linked_list.h"

struct Flag {
    char short_name;
    FixedString long_name;
    bool *value;
};

struct Option {
    char short_name;
    FixedString long_name;
    FixedString value;
};

struct Command {
    FixedString name;
    LinkedList<Option> options;
    LinkedList<Flag> flags;
};

struct CliApp {
    FixedString name;
    LinkedList<Command> sub_commands;
};

auto create_cli_app(const char* name, MemoryArena &arena) -> CliApp& {
    auto *app = static_cast<CliApp *>(arena.allocate(sizeof(CliApp)));
    app->name = FixedString::create(name, arena);
    return *app;
}

auto add_subcommand(CliApp &app, const char* name, MemoryArena &arena) -> Command& {
    Command subcommand;
    subcommand.name = FixedString::create(name, arena);
    return *app.sub_commands.insert(subcommand, arena);
}

auto add_flag(CliApp &app, const char short_name, const char* long_name, bool *value, MemoryArena &arena) -> Flag& {
    auto *flag = static_cast<Flag *>(arena.allocate(sizeof(Flag)));
    flag->short_name = short_name;
    flag->long_name = FixedString::create(long_name, arena);
    flag->value = value;
    return *flag;
}


auto parse_command(FixedString text, Command *root_command, MemoryArena &arena) -> LinkedList<FixedString>& {
    i32 start = 0;
    LinkedList<FixedString> result;
    /*for (int i = 0; i < text.len(); i++) {
        if (text[i] == ' ') {
            FixedString command_txt = FixedString::create(&text[start], i - start, g_transient);
            Command *curr_command = root_command->sub_commands;
            do {
                if (curr_command->name == command_txt) {
                    result.insert(command_txt, arena);
                    root_command = curr_command;
                    start = i+1;
                    break;
                }
                else {
                    curr_command = curr_command->next;
                }
            }
            while(curr_command != nullptr);
        }
    }*/

    return result;
}