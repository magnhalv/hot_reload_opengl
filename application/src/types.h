#ifndef HOT_RELOAD_OPENGL_TYPES_H
#define HOT_RELOAD_OPENGL_TYPES_H

#include <cstdint>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

constexpr u64 KiloBytes(u64 num_kb) { return num_kb * 1024; }
constexpr u64 MegaBytes(u64 num_mb) { return KiloBytes(1024 * num_mb); }
constexpr u64 GigaBytes(u64 num_gb) { return MegaBytes(1024 * num_gb); }



#endif //HOT_RELOAD_OPENGL_TYPES_H
