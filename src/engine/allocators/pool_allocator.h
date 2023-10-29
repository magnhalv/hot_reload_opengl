#ifndef HOT_RELOAD_OPENGL_POOL_ALLOCATOR_H
#define HOT_RELOAD_OPENGL_POOL_ALLOCATOR_H

#include <platform/types.h>

const u8 GUARD_SIZE = 4;
const u32 GUARD_WRITE_PATTERN = 0xEFBEADDE; // DEADBEEF

struct PoolAllocator {
    u64 num_pools;
    u64 pool_size;
    u8 *memory;
    u64 memory_size;

    u64 allocated_pools;

    [[nodiscard]] static auto calc_total_size(u64 req_num_pools, u64 req_pool_size) -> u64 {
        return (req_pool_size + GUARD_SIZE) * req_num_pools + GUARD_SIZE;
    }

    auto init(u64 in_num_pools, u64 in_pool_size, void *in_memory, u64 in_memory_size) -> void {
        assert(in_num_pools <= 64);
        assert(in_memory_size == calc_total_size(in_num_pools, in_pool_size));
        num_pools = in_num_pools;
        pool_size = in_pool_size;
        memory = static_cast<u8*>(in_memory);
        memory_size = in_memory_size;
        allocated_pools = 0;

        u32* value = reinterpret_cast<u32*>(memory);
        *value = GUARD_WRITE_PATTERN;
        for (auto i = 0; i < num_pools; i++) {
            u64 pos = ((pool_size + GUARD_SIZE) * i) + pool_size + GUARD_SIZE;
            value = reinterpret_cast<u32*>(&memory[pos]);
            *value = GUARD_WRITE_PATTERN;
        }
    }

    auto check_integrity() const -> void {
        for (auto i = 0; i < num_pools; i++) {
            u64 pos = ((pool_size + GUARD_SIZE) * i) + pool_size + GUARD_SIZE;
            u32* value = reinterpret_cast<u32*>(&memory[pos]);
            assert(*value == GUARD_WRITE_PATTERN);
        }
    }
};

#endif //HOT_RELOAD_OPENGL_POOL_ALLOCATOR_H