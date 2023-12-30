#ifndef HOT_RELOAD_OPENGL_LOGGER_H
#define HOT_RELOAD_OPENGL_LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

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

inline void crash_and_burn(const char *msg, ...) {
    va_list args;
    va_start(args, msg);

    va_list args_copy;
    va_copy(args_copy, args);
    printf("\033[31m");
    log("[CRITICAL ERROR]: ", msg, args_copy);
    printf("\033[0m");
    va_end(args_copy);

    va_end(args);
    std::exit(1);
}

#endif //HOT_RELOAD_OPENGL_LOGGER_H
