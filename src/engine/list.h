#pragma once

#include "math/vec2.h"
#include "math/vec3.h"
#include "memory_arena.h"
#include <cassert>
#include <platform/types.h>

// FixedList
template <typename T> struct FList {

  explicit FList() : _size(0), _data(nullptr), _max_size(0) {
  }

  ~FList() = default;

  auto init(MemoryArena& arena, size_t max_size) -> void {
    _data = allocate<T>(arena, max_size);
    _size = 0;
    _max_size = max_size;
  }

  T& operator[](size_t index) {
    assert(index < _size);
    return _data[index];
  }

  const T& operator[](size_t index) const {
    assert(index < _size);
    return _data[index];
  }

  [[nodiscard]] auto inline data() const -> T* {
    return _data;
  }

  [[nodiscard]] auto inline size() const -> size_t {
    return _size;
  }

  auto inline empty() -> void {
    _size = 0;
  }

  [[nodiscard]] auto inline is_empty() -> bool {
    return _size == 0;
  }

  auto inline push(T* value) -> void {
    assert(_size < _max_size);
    _data[_size++] = *value;
  }

  auto inline push(T value) -> void {
    assert(_size < _max_size);
    _data[_size++] = value;
  }

  auto inline push_range(const T* value, size_t length) -> T* {
    assert(_size + length <= _max_size);
    memcpy(_data + _size, value, length);
    auto result = &_data[_size];
    _size += length;
    return result;
  }

  auto inline pop(size_t num = 1) -> void {
    assert(_size - num >= 0);
    _size -= num;
  }

  class ListIterator {
    private:
    T* ptr;

    public:
    explicit ListIterator(T* ptr) : ptr(ptr) {
    }

    ListIterator& operator++() {
      ++ptr;
      return *this;
    }

    bool operator!=(const ListIterator& other) const {
      return ptr != other.ptr;
    }

    T& operator*() const {
      return *ptr;
    }
  };

  [[nodiscard]] ListIterator begin() const {
    return ListIterator(_data);
  }

  [[nodiscard]] ListIterator end() const {
    return ListIterator(_data + _size);
  }

  [[nodiscard]] inline auto is_full() -> bool {
    return _size == _max_size;
  }

  private:
  size_t _max_size{};
  size_t _size;
  T* _data;
};

extern template class FList<i32>;

extern template class FList<f32>;

extern template class FList<vec3>;

extern template class FList<vec2>;
