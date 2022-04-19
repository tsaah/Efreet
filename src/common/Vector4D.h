#pragma once

#include "common.h"
#include "Vector2D.h"
#include "Vector3D.h"

namespace efreet {

template<class T>
struct Vector4D {
    union {
        T data[4] = {};
        struct {
            union {
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
                Vector3D<T> xyz;
                Vector3D<T> rgb;
            };
            union {
                T w;
                T a;
            };
        };
        struct {
            union {
                struct {
                    T left;
                    T top;
                };
                Vector2D<T> topLeft;
                Vector2D<T> position;
            };
            union {
                struct {
                    T width;
                    T height;
                };
                Vector2D<T> size;
            };
        };
    };

    Vector4D() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(0)) {}
    Vector4D(T v) : x(static_cast<T>(v)), y(static_cast<T>(v)), z(static_cast<T>(v)), w(static_cast<T>(v)) {}
    Vector4D(T xa, T ya, T za, T wa) : x(static_cast<T>(xa)), y(static_cast<T>(ya)), z(static_cast<T>(za)), w(static_cast<T>(wa)) {}
    Vector4D(const Vector2D<T>& xya): xy(xya), z(static_cast<T>(0)), w(static_cast<T>(0)) {}
    Vector4D(const Vector3D<T>& xyza): xyz(xyza), w(static_cast<T>(0)) {}
    Vector4D(const Vector4D<T>& other): xyz(other.xyz), w(other.w) {}
    Vector4D(const Vector2D<T>& positiona, Vector2D<T>& sizea): position(positiona), size(sizea) {}
};

using v4 = Vector4D<f32>;
using v4i = Vector4D<i32>;

} // namespace efreet
