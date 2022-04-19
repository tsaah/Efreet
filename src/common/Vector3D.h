#pragma once

#include "common.h"
#include "Vector2D.h"

namespace efreet {

template<class T>
struct Vector3D {
    union {
        T data[3] = {};
        struct {
            union {
                struct {
                    T x;
                    T y;
                };
                struct {
                    T r;
                    T g;
                };
                Vector2D<T> xy;
                Vector2D<T> rg;
            };
            union {
                T z;
                T b;
            };
        };
    };

    Vector3D() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}
    Vector3D(T v) : x(static_cast<T>(v)), y(static_cast<T>(v)), z(static_cast<T>(v)) {}
    Vector3D(T xa, T ya, T za) : x(static_cast<T>(xa)), y(static_cast<T>(ya)), z(static_cast<T>(za)) {}
    Vector3D(const Vector2D<T>& xya) : xy(xya), z(static_cast<T>(0)) {}
    Vector3D(const Vector3D<T>& other) : xy(other.xy), z(other.z) {}
};

using v3 = Vector3D<f32>;

} // namespace efreet