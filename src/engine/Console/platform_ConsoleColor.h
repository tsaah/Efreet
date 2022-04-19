#pragma once

#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include <common.h>

#include <Windows.h>

namespace efreet::engine::platform::console {

enum class Color: u8 {
    COLOR_FATAL = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY,
    COLOR_ERROR = FOREGROUND_RED | FOREGROUND_INTENSITY,
    COLOR_WARN = FOREGROUND_GREEN | FOREGROUND_RED,
    COLOR_INFO = FOREGROUND_GREEN,
    COLOR_DEBUG = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
    COLOR_TRACE = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED
};

} // namespace efreet::engine::platform::console

#else
#error "that platform is not implemented"
#endif