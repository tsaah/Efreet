#include "sandbox.h"

#include <iostream>
#include <string>

#include <Windows.h>

namespace efreet::module {

namespace {

const char* moduleName = "Sandbox";
const char* moduleDescription = "Just a sandbox module for testing";
i32 moduleVersion = 0;

} // namespace

const char* Sandbox::name() {
    return moduleName;
}

const char* Sandbox::description() {
    return moduleDescription;
}

u32 Sandbox::version() {
    return moduleVersion;
}

b32 Sandbox::initialize(const ModuleConfig& config) {
    E_ASSERT_DEBUG(::GetLastError() == 0);
    LoggerProvider::setLogger(config.logger, "SANDBOX");
    E_ASSERT_DEBUG(::GetLastError() == 0);
    // E_INFO("Module %s is initialized", name());
    if (efreet::LoggerProvider::logger() != nullptr) {
        E_INFO("Module %s is initialized",name());
    };
    const auto le = ::GetLastError();
    E_ASSERT_DEBUG(::GetLastError() == 0);
    return true;
}

void Sandbox::updateAndRender(const ModuleContext& context) {

}

} // namespace efreet::module