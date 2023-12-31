#pragma once

#include <cassert>
#include <platform/types.h>
#include "math/vec3.h"
#include "math/vec2.h"
#include "memory_arena.h"

template<typename T>
struct Array {
    Array() : _size(0), _data(nullptr) {}
    Array(T *values, size_t size) : _data{values}, _size{size} {}
    ~Array() = default;

    auto init(T *values, size_t size) -> void {
        _data = values;
        _size = size;
    }

    auto init(MemoryArena &arena, size_t size) -> void {
        _data = push_array<T>(&arena, size);
        _size = size;
    }

    T &operator[](size_t index) {
        assert(index < _size);
        return _data[index];
    }

    const T &operator[](size_t index) const {
        assert(index < _size);
        return _data[index];
    }

    [[nodiscard]] auto inline data() const -> T* {
        return _data;
    }

    [[nodiscard]] auto inline size() const -> size_t {
        return _size;
    }

    class ArrayIterator {
    private:
        T* ptr;

    public:
        explicit ArrayIterator(T* ptr) : ptr(ptr) {}

        ArrayIterator& operator++() {
            ++ptr;
            return *this;
        }

        bool operator!=(const ArrayIterator& other) const {
            return ptr != other.ptr;
        }

        T& operator*() const {
            return *ptr;
        }
    };

    [[nodiscard]] ArrayIterator begin() const {
        return ArrayIterator(_data);
    }

    [[nodiscard]] ArrayIterator end() const {
        return ArrayIterator(_data + _size);
    }

private:
    size_t _size;
    T *_data;
};

extern template
class Array<i32>;

extern template
class Array<f32>;

extern template
class Array<vec3>;

extern template
class Array<vec2>;
