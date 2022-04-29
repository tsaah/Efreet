#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "platform_Window.h"

#include <Common.h>

#include <vector>

namespace efreet::engine::platform::window {

namespace {

const v2i DEFAULT_POSITION = { CW_USEDEFAULT, CW_USEDEFAULT };
const v2i DEFAULT_CLIENT_SIZE = { CW_USEDEFAULT, CW_USEDEFAULT };
const char* WINDOW_CLASS_NAME = "EfreetWindowClass";

struct State final {
    ::HWND handle{ nullptr };
    v2i position{ DEFAULT_POSITION };
    v2i clientSize{ DEFAULT_CLIENT_SIZE };
    v2i fullscreenSize{};
    ::DWORD style{ WS_VISIBLE };
    b32 isFullscreen{ false };
    b32 isClosed{ false };
};

b32 windowClassRegistered_{ false };
std::vector<State> windowStateVector_;
std::vector<WindowId> availableSlots_;

State& getStateFromId(WindowId id) {
    E_ASSERT(id < windowStateVector_.size());
    E_ASSERT(windowStateVector_[id].handle != nullptr);
    return windowStateVector_[id];
}

State& getStateFromHandle(::HWND handle) {
    const WindowId id{ static_cast<WindowId>(::GetWindowLongPtrA(handle, GWLP_USERDATA)) };
    return getStateFromId(id);
}

WindowId storeWindowState(const State& state) {
    WindowId id;
    if (availableSlots_.empty()) {
        id = static_cast<WindowId>(windowStateVector_.size());
        windowStateVector_.emplace_back(state);
    } else {
        id = availableSlots_.back();
        availableSlots_.pop_back();
        windowStateVector_[id] = state;
    }
    return id;
}

void removeWindowState(WindowId id) {
    E_ASSERT(id < windowStateVector_.size());
    availableSlots_.emplace_back(id);
}

::LRESULT CALLBACK internalWindowProc(::HWND windowHandle, u32 message, ::WPARAM wParam, ::LPARAM lParam) {
    DEBUG_OP(if (::GetLastError() == 1400) { ::SetLastError(0); });
    E_ASSERT_DEBUG(::GetLastError() == 0);

    State* state{ nullptr };
    switch (message) {
        case WM_DESTROY: {
            getStateFromHandle(windowHandle).isClosed = true;
        } break;
        case WM_EXITSIZEMOVE: {
            state = &getStateFromHandle(windowHandle);
        } break;
        case WM_SIZE: {
            if (wParam == SIZE_MAXIMIZED) {
                state = &getStateFromHandle(windowHandle);
            }
        } break;
        case WM_SYSCOMMAND: {
            if (wParam == SC_RESTORE) {
                state = &getStateFromHandle(windowHandle);
            }
        } break;
        default: break;
    }

    if (state != nullptr) {
        E_ASSERT(state->handle != nullptr);
        ::RECT rc;
        const auto getClientRectResult = ::GetClientRect(state->handle, &rc);
        E_ASSERT(getClientRectResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);
        const v2i size = { rc.right - rc.left, rc.bottom - rc.top };
        if (state->isFullscreen) {
            state->fullscreenSize = size;
        } else {
            state->clientSize = size;
        }
    }

    auto callback = reinterpret_cast<Proc>(::GetWindowLongPtrA(windowHandle, 0));
    DEBUG_OP(if (::GetLastError() == 1413) { ::SetLastError(0); });
    E_ASSERT_DEBUG(::GetLastError() == 0);

    const auto result = (callback == nullptr)
        ? ::DefWindowProcA(windowHandle, message, wParam, lParam)
        : callback(windowHandle, message, wParam, lParam);

    DEBUG_OP(if (::GetLastError() == 1400) { ::SetLastError(0); });
    E_ASSERT_DEBUG(::GetLastError() == 0);

    return result;
}

void resize(State& state, const v2i& position, const v2i& size) {
    ::RECT windowRect{ 0, 0, size.width, size.height };
    const auto adjustWindowRectResult = ::AdjustWindowRect(&windowRect, state.style, FALSE);
    E_ASSERT(adjustWindowRectResult != 0);
    E_ASSERT_DEBUG(::GetLastError() == 0);
    const i32 width{ windowRect.right - windowRect.left };
    const i32 height{ windowRect.bottom - windowRect.top };
    const auto moveWindowResult = ::MoveWindow(state.handle, position.x, position.y, width, height, true);
    E_ASSERT(moveWindowResult != 0);
    E_ASSERT_DEBUG(::GetLastError() == 0);
}

} // namespace

Window create(const Config* const config) {
    Proc callback{ config == nullptr ? nullptr : config->callback };
    Handle parent{ config == nullptr ? nullptr : config->parent };

    if (!windowClassRegistered_) {
        ::WNDCLASSEXA windowClass = {};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = internalWindowProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = (callback == nullptr) ? 0 : sizeof(callback);
        windowClass.hInstance = nullptr;
        windowClass.hIcon = ::LoadIconA(nullptr, IDI_APPLICATION);
        E_ASSERT_DEBUG(::GetLastError() == 0);
        windowClass.hCursor = ::LoadCursorA(nullptr, IDC_ARROW);
        E_ASSERT_DEBUG(::GetLastError() == 0);
        windowClass.hbrBackground = ::CreateSolidBrush(RGB(76, 76, 127));
        E_ASSERT_DEBUG(::GetLastError() == 0);
        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = WINDOW_CLASS_NAME;
        windowClass.hIconSm = ::LoadIconA(nullptr, IDI_APPLICATION);
        E_ASSERT_DEBUG(::GetLastError() == 0);
        const auto registerClassResult = ::RegisterClassExA(&windowClass);
        E_ASSERT(registerClassResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);
        windowClassRegistered_ = registerClassResult != 0;
    }

    State state = {};
    if (config != nullptr) {
        state.position = config->position;
        state.clientSize = config->clientSize;
    } else {
        state.position = DEFAULT_POSITION;
        state.clientSize = DEFAULT_CLIENT_SIZE;
    }

    state.style |= WS_VISIBLE | (parent ? WS_CHILD : WS_OVERLAPPEDWINDOW);

    ::RECT rc{ state.position.x, state.position.y, state.position.x + state.clientSize.width, state.position.y + state.clientSize.height };
    {
        const auto adjustWindowRectResult = ::AdjustWindowRect(&rc, state.style, FALSE);
        E_ASSERT(adjustWindowRectResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);
    }

    const char* title = { (config == nullptr || config->title == nullptr) ? nullptr : config->title };
    const i32 left{ state.position.x == CW_USEDEFAULT ? CW_USEDEFAULT : rc.left };
    const i32 top{ state.position.y == CW_USEDEFAULT ? CW_USEDEFAULT : rc.top };
    const i32 width{ state.clientSize.width == CW_USEDEFAULT ? CW_USEDEFAULT : (rc.right - rc.left) };
    const i32 height{ state.clientSize.height == CW_USEDEFAULT ? CW_USEDEFAULT : (rc.bottom - rc.top) };

    state.handle = ::CreateWindowExA(
        0,
        WINDOW_CLASS_NAME,
        title,
        state.style,
        left, top, width, height,
        parent, nullptr, nullptr, nullptr);

    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (state.handle == nullptr) {
        const auto error = ::GetLastError();
        E_FATAL("Window creation failed. Last error: %d", error);
        ::MessageBoxA(nullptr, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return {};
    }

    WindowId id{ storeWindowState(state) };
    if (callback != nullptr) {
        ::SetWindowLongPtrA(state.handle, GWLP_USERDATA, static_cast<LONG_PTR>(id));
        E_ASSERT_DEBUG(::GetLastError() == 0);

        ::SetWindowLongPtrA(state.handle, 0, reinterpret_cast<::LONG_PTR>(callback));
        E_ASSERT_DEBUG(::GetLastError() == 0);

        ::ShowWindow(state.handle, SW_SHOWNORMAL);
        E_ASSERT_DEBUG(::GetLastError() == 0);

        const auto updateWindowResult = ::UpdateWindow(state.handle);
        E_ASSERT(updateWindowResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);

        E_TRACE("created window %u (0x%x)", id, state.handle);

        return Window{ id };
    }

    return {};
}

void destroy(WindowId id) {
    E_ASSERT_DEBUG(::GetLastError() == 0);
    auto& state = getStateFromId(id);
    E_TRACE("destroyed window %u (0x%x)", id, state.handle);
    const auto destroyWindowResult = ::DestroyWindow(state.handle);
    state.handle = nullptr;
    // E_ASSERT(destroyWindowResult != 0);
    DEBUG_OP(if (::GetLastError() == 1400) { ::SetLastError(0); });
    E_ASSERT_DEBUG(::GetLastError() == 0);
    removeWindowState(id);
}

void setFullscreen(WindowId id, b32 fullscreen) {
    State& state{ getStateFromId(id) };

    if (state.isFullscreen != fullscreen) {
        state.isFullscreen = fullscreen;

        if (fullscreen) {
            ::RECT clientRect;
            ::GetClientRect(state.handle, &clientRect);
            state.clientSize = { clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };
            ::RECT windowRect;
            ::GetWindowRect(state.handle, &windowRect);
            state.position = { windowRect.left, windowRect.top };
            state.style = 0;
            ::SetWindowLongPtrA(state.handle, GWL_STYLE, 0);
            ::ShowWindow(state.handle, SW_MAXIMIZE);
        } else {
            ::SetWindowLongPtrA(state.handle, GWL_STYLE, state.style);
            resize(state, state.position, state.clientSize);
            ::ShowWindow(state.handle, SW_SHOWNORMAL);
        }
    }
}

b32 isFullscreen(WindowId id) {
    return getStateFromId(id).isFullscreen;
}

void* handle(WindowId id) {
    return getStateFromId(id).handle;
}

void setTitle(WindowId id, const char* title) {
    ::SetWindowTextA(getStateFromId(id).handle, title);
}

v2i position(WindowId id) {
    State& state{ getStateFromId(id) };
    return state.isFullscreen ? v2i{} : state.position;
}

v2i size(WindowId id) {
    State& state{ getStateFromId(id) };
    return state.isFullscreen ? state.fullscreenSize : state.clientSize;
}

void resize(WindowId id, const v2i& size) {
    State& state{ getStateFromId(id) };
    resize(state, state.isFullscreen ? v2i{} : state.position, size);
}

void resize(WindowId id, const v4i& rect) {
    State& state{ getStateFromId(id) };
    resize(state, rect.position, rect.size);
}

b32 isClosed(WindowId id) {
    return getStateFromId(id).isClosed;
}

} // namespace efreet::engine::platform::window

#else
#error "that platform is not implemented"
#endif


