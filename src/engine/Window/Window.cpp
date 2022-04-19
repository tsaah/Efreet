#include "Window.h"

#include "platform_Window.h"

namespace efreet::engine {

void Window::setFullscreen(b32 fullscreen) const {
    E_ASSERT(isValid());
    platform::window::setFullscreen(id_, fullscreen);
}

b32 Window::isFullscreen() const {
    E_ASSERT(isValid());
    return platform::window::isFullscreen(id_);
}

void* Window::handle() const {
    E_ASSERT(isValid());
    return reinterpret_cast<void*>(platform::window::handle(id_));
}

void Window::setTitle(const char* title) const {
    E_ASSERT(isValid());
    platform::window::setTitle(id_, title);
}

v2i Window::position() const {
    E_ASSERT(isValid());
    return platform::window::position(id_);
}

v2i Window::size() const {
    E_ASSERT(isValid());
    return platform::window::size(id_);
}

void Window::resize(const v2i& size) const {
    E_ASSERT(isValid());
    platform::window::resize(id_, size);
}

void Window::resize(const v4i& rect) const {
    E_ASSERT(isValid());
    platform::window::resize(id_, rect);
}

b32 Window::isClosed() const {
    E_ASSERT(isValid());
    return platform::window::isClosed(id_);
}

} // namespace efreet::engine