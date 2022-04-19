#pragma once

#include <ILogger.h>

namespace efreet {

struct ModuleConfig final {
    ILogger* logger{ nullptr };
};

} // namespace efreet
