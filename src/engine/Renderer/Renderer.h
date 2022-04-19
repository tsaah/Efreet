#pragma once

#include <common.h>

#include <Window/Window.h>

namespace efreet::engine::renderer {

class Surface {
public:
private:
};

struct RenderSurface {
    Window window{};
    Surface surface{};
};


class Renderer final {
public:
    b32 init(const char* applicationName);
    void cleanup();

private:

};

} // namespace efreet::engine::renderer
