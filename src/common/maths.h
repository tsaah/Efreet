#pragma once

namespace efreet {

template <class T>
inline constexpr T clamp(T val, T min,T max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

} // namespace