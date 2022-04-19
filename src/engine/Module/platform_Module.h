#pragma once

#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include <common.h>
#include <IModule.h>

namespace efreet::engine::platform::module {

IModule* load(const char* fileName);
void unload(IModule*& module);

} // namespace efreet::engine::platform::module

#else
#error "that platform is not implemented"
#endif