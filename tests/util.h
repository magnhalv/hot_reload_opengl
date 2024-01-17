#pragma once

#include "doctest.h"
#include "logger.h"
#include <cstring>

#include <csetjmp>


#define CHECK_CRASH(expr, msg)                    \
      do {                                        \
        jmp_buf buf;                              \
        auto has_crashed = false;                 \
        if (setjmp(buf) == 0) {                        \
             set_crash_jump(&buf);                \
             expr;                                \
             CHECK(has_crashed);                  \
        }                                         \
        else {                                    \
           has_crashed = true;                    \
        }                                         \
        CHECK(has_crashed);                       \
        printf("Msg: %s\n", global_crash_message);       \
        CHECK(global_has_crashed);                                          \
        CHECK(strcmp(msg, global_crash_message) == 0); \
      } while(0)



