#pragma once

#include <platform/types.h>

inline auto min(f32 a, f32 b) -> f32 {
    return a > b ? b : a;
}

inline auto max(f32 a, f32 b) -> f32 {
    return a > b ? a : b;
}