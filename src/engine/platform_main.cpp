#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include <common.h>
#include <Application/Application.h>
#include <Logger/Logger.h>

#ifdef _DEBUG

#include <crtdbg.h>

int main(int argc, char** argv) {
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#else

#include <Windows.h>

int WINAPI WinMain(::HINSTANCE, ::HINSTANCE, ::LPSTR, int) {

#endif

    efreet::LoggerProvider::setLogger(&efreet::engine::Logger::instance(), "ENGINE");

    return efreet::engine::platform::application::exec();
}
#else
#error "that platform is not implemented"
#endif