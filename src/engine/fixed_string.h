#pragma once

#include <platform/types.h>
#include <array.h>
#include <cstring>
#include <cassert>

#include "memory_arena.h"

struct FixedString {
    // TODO: Remove this, too risky
    [[nodiscard]] static auto create(const char *s, MemoryArena &arena) -> FixedString {
        auto length = strlen(s);
        return FixedString::create(s, length, arena);
    }

    [[nodiscard]] static auto create(const char *s, u32 length, MemoryArena &arena) -> FixedString {
        char *data = static_cast<char *>(arena.allocate(strlen(s) + 1));
        for (auto i = 0; i < length; i++) {
            data[i] = s[i];
        }
        data[length] = '\0';

        return FixedString{
                ._data = data,
                ._length = length
        };
    }

    [[nodiscard]] static inline auto create(const char *s, u32 length, MemoryArena *arena) -> FixedString {
        return FixedString::create(s, length, *arena);
    }

    [[nodiscard]] inline auto data() const -> const char* {
        return _data;
    }

    [[nodiscard]] auto substring(i32 start, i32 length, MemoryArena &arena) const -> FixedString {
        assert(start >= 0);
        assert(length >= 0);
        assert (start + length <= len());
        return FixedString::create(&_data[start], length, arena);

    }

    const char &operator[](size_t index) const {
        assert(index < _length);
        return _data[index];
    }

    bool operator==(const FixedString other) const {
        if (_length != other.len()) {
            return false;
        }
        for (auto i = 0; i < _length; i++) {
            if (_data[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator==(const char* other) const {
        auto other_len = strlen(other);
        if (_length != other_len) {
            return false;
        }
        for (auto i = 0; i < _length; i++) {
            if (_data[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] inline auto len() const -> u32 {
        return _length;
    }

    char *_data;
    u32 _length;
};

auto inline split(FixedString str, char delimiter, MemoryArena &arena) -> Array<FixedString>& {
    i32 num_entries = 0;
    bool has_non_delimiter_values;
    // "" -> 0
    // "test" -> 1
    // "test test" -> 2
    // " test test " -> 2
    // " test      test " -> 2
    for (auto i = 0; i < str.len(); i++) {
        // Has a non-delimiter value
        if (str[i] != delimiter && num_entries == 0) {
            num_entries = 1;
        }
        if (str[i] == delimiter) {
            if ((i < str.len() - 2) && str[i+1] != delimiter) {
                num_entries++;
            }
        }
    }

    Array<FixedString> &result = *Array<FixedString>::create(num_entries, arena);
    if (result.size() == 0) {
        return result;
    }

    i32 start_idx = -1;
    i32 last_entry_idx = 0;
    for (auto i = 0; i < str.len(); i++) {
        if (str[i] != delimiter && start_idx == -1) {
            start_idx = i;
        }
        if ((str[i] == delimiter) && start_idx != -1) {
            result[last_entry_idx] = str.substring(start_idx, i - start_idx, arena);
            last_entry_idx++;
            start_idx = -1;
        }
        if (i == (str.len()-1) && start_idx != -1) {
            result[last_entry_idx] = str.substring(start_idx, (i+1) - start_idx, arena);
        }
    }

    return result;
}