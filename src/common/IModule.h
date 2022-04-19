#pragma once

#include "basic_types.h"
#include "module_export.h"
#include "ModuleContext.h"
#include "ModuleConfig.h"

namespace efreet {

class MODULE_EXPORT IModule {
public:
    virtual ~IModule() = default;

    virtual const char* name() = 0;
    virtual const char* description() = 0;
    virtual u32 version() = 0;
    virtual b32 initialize(const ModuleConfig& config) = 0;
    virtual void updateAndRender(const ModuleContext& context) = 0;
};

} // namespace efreet