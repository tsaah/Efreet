#pragma once

#include "basic_types.h"

#include "IModule.h"

extern "C" {

namespace efreet {

MODULE_EXPORT IModule* createModule();
MODULE_EXPORT void destroyModule(IModule* module);

} // namespace efreet

} // extern "C"