#pragma once

#include <common.h>
#include <module_export.h>
#include <IModule.h>

namespace efreet::module {

class MODULE_EXPORT Sandbox final: public IModule {
public:
    const char* name() override;
    const char* description() override;
    u32 version() override;
    b32 initialize(const ModuleConfig& config) override;
    void updateAndRender(const ModuleContext& context) override;
};

} // namespace efreet::module