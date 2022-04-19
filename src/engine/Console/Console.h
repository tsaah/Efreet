#pragma once

#include "platform_ConsoleColor.h"

#include <common.h>

namespace efreet::engine::platform::console {

void write(const char* message, Color color);
void writeError(const char* message, Color color);

} // namespace efreet::engine::platform::console
