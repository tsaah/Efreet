#pragma once

#include "static_assert.h"

#include <cstddef>

namespace efreet {

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

using b32 = i32;

STATIC_ASSERT(sizeof(u8) == 1, "Expected size of u8 should be 1 byte");
STATIC_ASSERT(sizeof(u16) == 2, "Expected size of u8 should be 2 bytes");
STATIC_ASSERT(sizeof(u32) == 4, "Expected size of u8 should be 4 bytes");
STATIC_ASSERT(sizeof(u64) == 8, "Expected size of u8 should be 8 bytes");

STATIC_ASSERT(sizeof(i8) == 1, "Expected size of u8 should be 1 byte");
STATIC_ASSERT(sizeof(i16) == 2, "Expected size of u8 should be 2 bytes");
STATIC_ASSERT(sizeof(i32) == 4, "Expected size of u8 should be 4 bytes");
STATIC_ASSERT(sizeof(i64) == 8, "Expected size of u8 should be 8 bytes");

STATIC_ASSERT(sizeof(f32) == 4, "Expected size of u8 should be 4 bytes");
STATIC_ASSERT(sizeof(f64) == 8, "Expected size of u8 should be 8 bytes");

constexpr auto kilobytes(const auto value) { return value * 1024i64; }
constexpr auto megabytes(const auto value) { return kilobytes(value) * 1024i64; }
constexpr auto gigabytes(const auto value) { return megabytes(value) * 1024i64; }
constexpr auto terabytes(const auto value) { return gigabytes(value) * 1024i64; }

} // namespace efreet
