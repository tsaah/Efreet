#pragma once

#include <common.h>

namespace efreet {

template<class T>
struct Vector2D {
    union {
        T data[2] = {};
        struct {
            T x;
            T y;
        };
        struct {
            T width;
            T height;
        };
    };

    Vector2D() : x(static_cast<T>(0)), y(static_cast<T>(0)) {}
    Vector2D(T v) : x(static_cast<T>(v)), y(static_cast<T>(v)) {}
    Vector2D(T xa, T ya) : x(static_cast<T>(xa)), y(static_cast<T>(ya)) {}
    Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}
};

using v2 = Vector2D<f32>;
using v2i = Vector2D<i32>;

} // namespace efreet