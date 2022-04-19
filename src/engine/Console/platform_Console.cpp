#include <platform_detection.h>
#ifdef EFREET_PLATFORM_WINDOWS

#include "Console.h"

#include <asserts.h>

#include <Windows.h>

namespace efreet::engine::platform::console {

void write(const char* message, Color color) {
    DEBUG_OP(::SetLastError(0));

    auto consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    {
        E_PURE_ASSERT(consoleHandle != INVALID_HANDLE_VALUE);
        E_PURE_ASSERT(consoleHandle != nullptr);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto setConsoleTextAttributeResult = ::SetConsoleTextAttribute(consoleHandle, static_cast<u8>(color));
        E_PURE_ASSERT(setConsoleTextAttributeResult != 0);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        ::OutputDebugStringA(message);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto length = static_cast<::DWORD>(::strlen(message)); // TODO: strlen
        ::DWORD numberWritten{ 0 };
        const auto writeConsoleResult = ::WriteConsoleA(consoleHandle, message, length, &numberWritten, nullptr);
        E_PURE_ASSERT(writeConsoleResult != 0);
        E_PURE_ASSERT(numberWritten == length);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto setConsoleTextAttributeResult = ::SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        E_PURE_ASSERT(setConsoleTextAttributeResult != 0);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }
}

void writeError(const char* message, Color color) {
    DEBUG_OP(::SetLastError(0));

    auto consoleHandle = ::GetStdHandle(STD_ERROR_HANDLE);
    {
        E_PURE_ASSERT(consoleHandle != INVALID_HANDLE_VALUE);
        E_PURE_ASSERT(consoleHandle != nullptr);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto setConsoleTextAttributeResult = ::SetConsoleTextAttribute(consoleHandle, static_cast<u8>(color));
        E_PURE_ASSERT(setConsoleTextAttributeResult != 0);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        ::OutputDebugStringA(message);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto length = static_cast<::DWORD>(::strlen(message)); // TODO: strlen
        ::DWORD numberWritten{ 0 };
        const auto writeConsoleResult = ::WriteConsoleA(consoleHandle, message, length, &numberWritten, nullptr);
        E_PURE_ASSERT(writeConsoleResult != 0);
        E_PURE_ASSERT(numberWritten == length);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }

    {
        const auto setConsoleTextAttributeResult = ::SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        E_PURE_ASSERT(setConsoleTextAttributeResult != 0);
        E_PURE_ASSERT_DEBUG(::GetLastError() == 0);
    }
}

} // namespace efreet::engine::platform::console

#else
#error "that platform is not implemented"
#endif