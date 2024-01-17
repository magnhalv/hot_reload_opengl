#ifndef HOT_RELOAD_OPENGL_LOGGER_H
#define HOT_RELOAD_OPENGL_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <csetjmp>

extern char global_crash_message[512];
extern bool global_has_crashed;

extern jmp_buf *crash_jump;

inline void log(const char *type, const char *msg, va_list args) {
    printf("%s", type);
    vprintf(msg, args);
    printf("\n");
}

inline void log_info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    va_list args_copy;
    va_copy(args_copy, args);
    log("[INFO]: ", msg, args_copy);
    va_end(args_copy);

    va_end(args);
}

inline void log_error(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    va_list args_copy;
    va_copy(args_copy, args);
    log("[ERROR]: ", msg, args_copy);
    va_end(args_copy);

    va_end(args);
}

auto set_crash_jump(jmp_buf *exit_jump) -> void;
auto crash_and_burn(const char *msg, ...) -> void;

#endif //HOT_RELOAD_OPENGL_LOGGER_H
