#pragma once

#include "basic_types.h"

namespace efreet {

enum class LogLevel: u8 {
    LVL_FATAL,
    LVL_ERROR,
    LVL_WARN,
    LVL_INFO,
    LVL_DEBUG,
    LVL_TRACE
};

} // namespace efreet