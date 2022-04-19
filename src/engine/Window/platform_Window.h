#pragma once

#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "Window.h"

#include <Windows.h>

namespace efreet::engine::platform::window {

using Proc = ::LRESULT(*)(::HWND, ::UINT, ::WPARAM, ::LPARAM);
using Handle = ::HWND;

struct Config final {
    Proc callback{ nullptr };
    Handle parent{ nullptr };
    const char* title{ nullptr };
    v2i position{ CW_USEDEFAULT, CW_USEDEFAULT };
    v2i clientSize{ CW_USEDEFAULT, CW_USEDEFAULT };
};

Window create(const Config* const config = nullptr);
void destroy(WindowId);

void setFullscreen(WindowId id, b32 fullscreen);
b32 isFullscreen(WindowId id);
void* handle(WindowId id);
void setTitle(WindowId id, const char* title);
v2i position(WindowId id);
v2i size(WindowId id);
void resize(WindowId id, const v2i& size);
void resize(WindowId id, const v4i& rect);
b32 isClosed(WindowId id);

} // namespace efreet::engine::platform::window

#else
#error "that platform is not implemented"
#endif