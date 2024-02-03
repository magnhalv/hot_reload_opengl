#pragma once

#pragma once

#include <cassert>
#include <platform/types.h>
#include "math/vec3.h"
#include "math/vec2.h"
#include "memory_arena.h"

// FixedList
template<typename T>
struct GList {

    explicit GList() : _size(0), _data(nullptr) {}

    ~GList() = default;

    auto init(MemoryArena &arena) -> void {
        _arena = &arena;
        _data = nullptr;
        _size = 0;
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

    auto inline empty() -> void {
        _size = 0;
    }

    [[nodiscard]] auto inline is_empty() -> bool {
        return _size == 0;
    }

    auto inline push(T &value) -> void {
        extend(_data, _arena, 1);
        _data[_size++] = value;
    }

    // TODO: Need to implement shrink first
    /*auto inline pop(size_t num = 1) -> void {
        assert(_size - num >= 0);
        _size -= num;
    }*/

    class ListIterator {
    private:
        T *ptr;

    public:
        explicit ListIterator(T *ptr) : ptr(ptr) {}

        ListIterator &operator++() {
            ++ptr;
            return *this;
        }

        bool operator!=(const ListIterator &other) const {
            return ptr != other.ptr;
        }

        T &operator*() const {
            return *ptr;
        }
    };

    [[nodiscard]] ListIterator begin() const {
        return ListIterator(_data);
    }

    [[nodiscard]] ListIterator end() const {
        return ListIterator(_data + _size);
    }

private:
    size_t _size;
    T *_data;
    MemoryArena *_arena;
};