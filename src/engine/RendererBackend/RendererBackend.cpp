#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "RendererBackend.h"

#include <asserts.h>

#include <Windows.h>

namespace efreet::engine::renderer::backend {

namespace {

::HMODULE dll_{ nullptr };
std::function<BackendType()> type_;
std::function<u32()> version_;
std::function<b8(const RendererBackendConfig&)> init_;
std::function<void()> cleanup_;

const char* getFilename(BackendType backend) {
    switch (backend) {
        case BackendType::OpenGL: return "erb_opengl.dll";
        case BackendType::Vulkan: return "erb_vulkan.dll";
        case BackendType::DirectX: return "erb_directx.dll";
        default: return "";
    }
}

} // namespace

b8 create(const RendererBackendConfig& config) {
    destroy();

    switch (config.backendType) {
        case BackendType::None: return false;
        case BackendType::Dummy: {
            // TODO: do dummy stuff
        } break;
        default: {
            const auto filename{ getFilename(config.backendType) };
            dll_ = ::LoadLibraryA(filename);
            DEBUG_OP(if (::GetLastError() == 126) { ::SetLastError(0); });
            E_ASSERT_DEBUG(::GetLastError() == 0);
            if (dll_ == nullptr) {
                destroy();
                E_FATAL("Could not load renderer backend library '%s'", filename);
                return false;
            }
        }
    }

    type_ = reinterpret_cast<BackendType(*)()>(::GetProcAddress(dll_, "backendType"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (type_ == nullptr) {
        destroy();
        return false;
    }

    version_ = reinterpret_cast<u32(*)()>(::GetProcAddress(dll_, "version"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (version_ == nullptr) {
        destroy();
        return false;
    }

    init_ = reinterpret_cast<b8(*)(const RendererBackendConfig&)>(::GetProcAddress(dll_, "init"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (init_ == nullptr) {
        destroy();
        return false;
    }

    cleanup_ = reinterpret_cast<void(*)()>(::GetProcAddress(dll_, "cleanup"));
    E_ASSERT_DEBUG(::GetLastError() == 0);
    if (cleanup_ == nullptr) {
        destroy();
        return false;
    }

    return init_(config);
}

void destroy() {
    if (cleanup_ != nullptr) {
        cleanup_();
    }
    if (dll_ != nullptr) {
        const auto freeLibraryResult = ::FreeLibrary(dll_); // TODO: check return value?
        E_ASSERT(freeLibraryResult != 0);
        E_ASSERT_DEBUG(::GetLastError() == 0);
    }
    dll_ = nullptr;
    type_ = nullptr;
    version_ = nullptr;
    init_ = nullptr;
    cleanup_ = nullptr;
}


BackendType type() { return type_(); }
u32 version() { return version_(); }
b8 init(const RendererBackendConfig& config) { return init_(config); }
void cleanup() { cleanup_(); }

} // namespace efreet::engine::renderer::backend

#else
#error "that platform is not implemented"
#endif