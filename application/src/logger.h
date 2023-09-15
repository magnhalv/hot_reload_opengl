#ifndef HOT_RELOAD_OPENGL_LOGGER_H
#define HOT_RELOAD_OPENGL_LOGGER_H

#include <cstdio>
#include <cstdarg>

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

#endif //HOT_RELOAD_OPENGL_LOGGER_H
