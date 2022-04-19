#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "platform_Module.h"

#include <asserts.h>
#include <ModuleApi.h>

#include <Windows.h>

namespace efreet::engine::platform::module {

namespace {

::HMODULE dll_{ nullptr };
IModule* module_{ nullptr };
void clearFunctions();
void destroyModule();

decltype(&efreet::createModule) createModule_{ nullptr };
decltype(&efreet::destroyModule) destroyModule_{ nullptr };

} // namespace

IModule* load(const char* fileName) {
    // TODO:: assert fileName

    unload(module_);

    dll_ = ::LoadLibraryA(fileName);
    DEBUG_OP(if (::GetLastError() == 126) { ::SetLastError(0); });
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (dll_ == nullptr) {
        E_FATAL("Could not load module '%s'", fileName);
        unload(module_);
        return nullptr;
    }

    createModule_ = reinterpret_cast<decltype(&efreet::createModule)>(::GetProcAddress(dll_, "createModule"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (createModule_ == nullptr) {
        unload(module_);
        return nullptr;
    }

    destroyModule_ = reinterpret_cast<decltype(&efreet::destroyModule)>(::GetProcAddress(dll_, "destroyModule"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (destroyModule_ == nullptr) {
        unload(module_);
        return nullptr;
    }

    module_ = createModule_();
    if (module_ == nullptr) {
        unload(module_);
        return nullptr;
    }

    return module_;
}

void unload(IModule*& module) {
    module = nullptr;
    if (module_ != nullptr) {
        delete module_;
    }
    module_ = nullptr;
    if (dll_ != nullptr) {
        const auto freeLibraryResult = ::FreeLibrary(dll_); // TODO: check return value?
        E_ASSERT(freeLibraryResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);
    }
    dll_ = nullptr;
    createModule_ = nullptr;
    destroyModule_ = nullptr;
}

} // namespace efreet::engine::platform::module

#else
#error "that platform is not implemented"
#endif