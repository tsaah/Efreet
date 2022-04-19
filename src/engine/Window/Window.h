#pragma once

#include "id.h"
#include <common.h>

namespace efreet::engine {

DEFINE_TYPED_ID(WindowId);

class Window final {
public:
    inline constexpr explicit Window(WindowId id): id_(id) {}
    inline constexpr Window(): id_(id::invalid) {}
    inline constexpr WindowId id() const { return id_; }
    inline constexpr b32 isValid() const { return id::isValid(id_); }

    void setFullscreen(b32 fullscreen) const;
    b32 isFullscreen() const;
    void* handle() const;
    void setTitle(const char* title) const;
    v2i position() const;
    v2i size() const;
    void resize(const v2i& size) const;
    void resize(const v4i& rect) const;
    b32 isClosed() const;

private:
    WindowId id_{ 0 };
};

} // namespace efreet::engine