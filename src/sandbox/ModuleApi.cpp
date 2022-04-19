#include <ModuleApi.h>
#include "sandbox.h"

extern "C" {

namespace efreet {

IModule* createModule() {
    return new module::Sandbox;
}

void destroyModule(IModule* module) {
    delete module;
}

} // namespace efreet

} // extern "C"