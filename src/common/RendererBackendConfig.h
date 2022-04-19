#pragma once

#include <basic_types.h>
#include <ILogger.h>

namespace efreet {

struct RendererBackendConfig {
    ILogger* logger{ nullptr };
    const char* applicationName{ nullptr };
};

} // namespace efreet