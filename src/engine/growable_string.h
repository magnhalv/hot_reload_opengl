#pragma once

#include <cassert>
#include <cstring>
#include <platform/types.h>

#include "logger.h"
#include "memory_arena.h"

/// @brief FixedString
struct GStr {
  [[nodiscard]] static auto create(const char* s, i32 max_length, MemoryArena& arena) -> GStr {
    auto length = strlen(s);
    char* data = static_cast<char*>(arena.allocate(max_length + 1));
    memcpy(data, s, length);
    data[length] = '\0';

    return GStr{ ._data = data, ._curr_length = static_cast<i32>(length), ._max_length = max_length };
  }

  [[nodiscard]] inline auto data() const -> const char* {
    return _data;
  }

  [[nodiscard]] auto substring(i32 start, i32 length, MemoryArena& arena) const -> GStr {
    assert(start >= 0);
    assert(length >= 0);
    assert(start + length <= len());
    return GStr::create(&_data[start], length, arena);
  }

  auto inline push(const char* s) {
    auto length = strlen(s);
    if (length + _curr_length > _max_length) {
      crash_and_burn("Exceeded max length of GStr");
    }
    memcpy(_data + _curr_length, s, length);
    _curr_length += length;
    _data[_curr_length] = '\0';
  }

  auto inline pop(i32 num = 1) {
    if (_curr_length - num < 0) {
      _curr_length = 0;
    } else {
      _curr_length -= num;
    }
    _data[_curr_length] = '\0';
  }

  const char& operator[](size_t index) const {
    assert(index < _curr_length);
    return _data[index];
  }

  bool operator==(const GStr other) const {
    if (_curr_length != other.len()) {
      return false;
    }
    return memcmp(_data, other._data, _curr_length) == 0;
  }

  bool operator==(const char* other) const {
    auto other_len = strlen(other);
    if (_curr_length != other_len) {
      return false;
    }
    for (auto i = 0; i < _curr_length; i++) {
      if (_data[i] != other[i]) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] inline auto len() const -> u32 {
    return _curr_length;
  }

  [[nodiscard]] inline auto is_full() const -> u32 {
    return _curr_length == _max_length;
  }

  char* _data;
  i32 _curr_length;
  i32 _max_length;
};
