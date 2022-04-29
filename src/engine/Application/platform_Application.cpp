#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "Application.h"

#include <IRendererBackend.h>
#include <Module/platform_Module.h>
#include <Renderer/Renderer.h>
#include <RendererBackend/RendererBackendFactory.h>
#include <RendererBackendConfig.h>
#include <Window/Window.h>
#include <Window/platform_Window.h>

#include <Windowsx.h>

namespace efreet::engine::platform::application {

namespace {

renderer::RenderSurface window_;
bool isRunning_{ false };
bool isSuspended_{ false };
IModule* module_{ nullptr };
renderer::IRendererBackend* rendererBackend_{ nullptr };

LRESULT applicationProc(::HWND windowHandle, u32 message, ::WPARAM wParam, ::LPARAM lParam) {
    switch (message) {
        case WM_DESTROY: {
            if (window_.window.isClosed()) {
                ::PostQuitMessage(0);
                E_ASSERT_DEBUG(::GetLastError() == 0);
                return 0;
            }
        } break;
        case WM_SYSCHAR: {
            if (wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN)) {
                window_.window.setFullscreen(!window_.window.isFullscreen());
                return 0;
            }
        } break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            const b8 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            // const auto k = static_cast<core::Input::Keys>(static_cast<u16>(wParam));
            // E_Input.processKey(k, pressed);
            E_TRACE("event key '%c' (0x%x) %s", static_cast<u16>(wParam), static_cast<u16>(wParam), pressed ? "pressed" : "released");
        } break;
        case WM_MOUSEMOVE: {
            // const v2i pos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            // E_Input.processMouseMove(pos);
            // E_TRACE("event mouse move '%ix%i'", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        } break;
        case WM_MOUSEWHEEL: {
            i32 zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (zDelta != 0) {
                zDelta = (zDelta < 0) ? -1 : 1;
                // E_Input.processMouseWheel(zDelta);
            }
            E_TRACE("event mouse wheel '%i'", zDelta);
        } break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: {
            const b32 pressed = message == WM_LBUTTONDOWN;
            // E_Input.processButton(E_Buttons::LEFT, pressed);
            E_TRACE("event mouse left button pressed: '%u'", pressed);
        } break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            const b32 pressed = message == WM_MBUTTONDOWN;
            // E_Input.processButton(E_Buttons::MIDDLE, pressed);
            E_TRACE("event mouse middle button pressed: '%u'", pressed);
        } break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP: {
            const b32 pressed = message == WM_RBUTTONDOWN;
            // E_Input.processButton(E_Buttons::RIGHT, pressed);
            E_TRACE("event mouse right button pressed: '%u'", pressed);
        } break;
        default: break;
    }

    return ::DefWindowProcA(windowHandle, message, wParam, lParam);
}

b32 init() {
    {
        module_ = module::load("sandbox.dll");
        if (module_ == nullptr) {
            return false;
        }
        const auto name = module_->name();
        const auto description = module_->description();
        const auto version = module_->version();

        ModuleConfig config;
        config.logger = LoggerProvider::logger();
        if (!module_->initialize(config)) {
            E_ASSERT_EXT(false, "Failed to initialize module");
        }
    }

    {
        window::Config config = {
            applicationProc,
            nullptr,
            module_->name(),
            { 100, 100 },
            { 320, 200 }
        };
        window_.window = window::create(&config);
    }

    {
        rendererBackend_ = renderer::RendererBackendFactory::create(renderer::RendererBackendFactory::Vulkan);
        if (rendererBackend_ == nullptr) {
            return false;
        }
        const auto name = rendererBackend_->name();
        const auto description = rendererBackend_->description();
        const auto version = rendererBackend_->version();

        RendererBackendConfig config;
        config.applicationName = module_->name();
        config.logger = LoggerProvider::logger();
        config.instance = nullptr;
        config.hwnd = (::HWND)(window_.window.handle());
        rendererBackend_->init(config);
    }

    // E_ASSERT(subscribe(Event::APPLICATION_QUIT));
    // E_ASSERT(subscribe(Event::KEY_PRESSED));
    // E_ASSERT(subscribe(Event::KEY_RELEASED));

    // if (!E_Platform.init()) {
    //     return false;
    // }

    isRunning_ = true;

    // E_TRACE("Trace");
    // E_DEBUG("Debug");
    // E_INFO("Info");
    // E_WARN("Warn");
    // E_ERROR("Error");
    // E_FATAL("Fatal");

    E_INFO("Efreet engine was initialized and has started");


    return true;
}

void shutdown() {
    rendererBackend_->cleanup();
    window::destroy(window_.window.id());
    module::unload(module_);
}

} // namespace

i32 exec() {
    const auto initResult = init();
    if (!initResult) {
        return 1;
    }
    // timer_.start();
    // timer_.update();
    // lastTime_ = timer_.elapsed();
    // i64 runningTime = 0.0;
    // u8 frameCount = 0;
    // const f64 targetFrameTimeSeconds = 1.0 / 60.0;
    // const i64 targetFrameTime = E_Platform.convertSecondsToTime(targetFrameTimeSeconds);

    while (isRunning_) {
        ::MSG message;
        E_ASSERT_DEBUG(::GetLastError() == 0);
        while (::PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE)) {
            DEBUG_OP(if (::GetLastError() == 1400) { ::SetLastError(0); });
            E_ASSERT_DEBUG(::GetLastError() == 0);
            ::TranslateMessage(&message);
            E_ASSERT_DEBUG(::GetLastError() == 0);
            ::DispatchMessageA(&message);
            E_ASSERT_DEBUG(::GetLastError() == 0);

            const auto& event = message.message;
            const auto& window = message.hwnd;
            const auto& wParam = message.wParam;
            const auto& lParam = message.lParam;

            switch (event) {
                case WM_QUIT: {
                    isRunning_ = false;
                } break;

                default: break;
            }
        }
        E_ASSERT_DEBUG(::GetLastError() == 0);
        if (isRunning_) {
            if (!isSuspended_) {
                // timer_.update();
                // const auto currentTime = timer_.elapsed();
                // const auto dt = E_Platform.convertTimeToSeconds(currentTime - lastTime_);

                // const auto frameStartTime = E_Platform.getAbsoluteTime();
                // if (!game_->update(dt)) {
                    // E_FATAL("Game update failed.");
                    // isRunning_ = FALSE;
                    // break;
                // }
                // if (!game_->render(dt)) {
                    // E_FATAL("Game render failed.");
                    // isRunning_ = FALSE;
                    // break;
                // }

                // const auto packet = std::make_unique<renderer::Renderer::RenderPacket>();
                // packet->dt_ = dt;
                // renderer_.drawFrame(packet);

                // const auto frameEndTime = E_Platform.getAbsoluteTime();;
                // const auto frameElapsedTime = frameEndTime - frameStartTime;
                // runningTime += frameElapsedTime;
                // const auto remainingTime = targetFrameTime - frameElapsedTime;

                // if (remainingTime > 0) {
                    // const auto sleepTime = static_cast<u64>(E_Platform.convertTimeToSeconds(remainingTime) * 1000);
                    // const b8 limitFrames = FALSE;
                    // if (sleepTime > 0 && limitFrames) {
                        // E_Platform.sleep(sleepTime - 1);
                    // }
                    // ++frameCount;
                // }

                // E_InputSystem.update(dt);
                // E_InputSystem.update(0.0f);

                // lastTime_ = currentTime;

                module_->updateAndRender({ 0.0f });

            }
        }
    }
    E_ASSERT_DEBUG(::GetLastError() == 0);

    shutdown();

    return 0;
}

} // namespace efreet::engine::platform::application

#else
#error "that platform is not implemented"
#endif