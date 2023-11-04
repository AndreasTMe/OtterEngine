#include "Otter.PCH.h"

#include "Platform/Platform.h"

#if OTR_PLATFORM_WINDOWS
#include "Platform/Windows/Platform.Win32.h"
#elif OTR_PLATFORM_LINUX
#include "Platform/Linux/PlatformLinux.h"
#else
#error "Unsupported platform"
#endif

namespace Otter
{
#if OTR_PLATFORM_WINDOWS

    Platform* Platform::CreatePlatform()
    {
        return New<Internal::WindowsPlatform>();
    }

    void Platform::DestroyPlatform(Platform* platform)
    {
        Delete((Internal::WindowsPlatform*) platform);
    }

    void* Platform::Allocate(UInt64 size)
    {
        OTR_LOG_TRACE("Requested allocation of {0} bytes from OS", size)
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0")

        void* block = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Failed to allocate memory")

        ZeroMemory(block, size);

        return block;
    }

    void* Platform::Reallocate(void* block, UInt64 size)
    {
        OTR_LOG_TRACE("Requested re-allocation of {0} bytes from OS", size)
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0")

        if (!block)
            return Allocate(size);

        void* newBlock = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, block, size);
        OTR_INTERNAL_ASSERT_MSG(newBlock != nullptr, "Failed to reallocate memory")

        return newBlock;
    }

    void Platform::Free(void* block)
    {
        OTR_LOG_TRACE("Requested de-allocation of memory from OS")
        OTR_INTERNAL_ASSERT(block != nullptr)

        HeapFree(GetProcessHeap(), 0, block);
    }

    void Platform::MemoryClear(void* block, UInt64 size)
    {
        OTR_INTERNAL_ASSERT(block != nullptr)

        ZeroMemory(block, size);
    }

    void Platform::Log(const char* message, UInt8 level)
    {
        OTR_INTERNAL_ASSERT(level < 6)

        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        OTR_INTERNAL_ASSERT(consoleHandle != INVALID_HANDLE_VALUE)

        // TRACE, DEBUG, INFO, WARN, ERROR, FATAL
        static UInt8 levels[6] = { 8, 7, 2, 6, 4, 64 };
        SetConsoleTextAttribute(consoleHandle, levels[level]);
        OutputDebugStringA(message);
        UInt64 length        = strlen(message);
        DWORD  numberWritten = 0;
        WriteConsoleA(consoleHandle,
                      message,
                      (DWORD) length,
                      &numberWritten,
                      nullptr);
    }

#elif OTR_PLATFORM_LINUX

    Platform* Platform::CreatePlatform()
    {
        return New<Internal::LinuxPlatform>();
    }

    void Platform::DestroyPlatform(Platform* platform)
    {
        Delete((Internal::LinuxPlatform*) platform);
    }

    void* Platform::Allocate(UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::Allocate' not supported for this platform")

        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0")

        void* block = malloc(size);

        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Failed to allocate memory")

        return nullptr;
    }

    void* Platform::Reallocate(void* block, UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::Reallocate' not supported for this platform")

        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0")

        if (!block)
            return Allocate(size);

//        void* newBlock = realloc(block, size);
//        OTR_INTERNAL_ASSERT_MSG(newBlock != nullptr, "Failed to reallocate memory")

        return nullptr;
    }

    void Platform::Free(void* block)
    {
        OTR_LOG_FATAL("'Platform::Free' not supported for this platform")

        OTR_INTERNAL_ASSERT(block != nullptr)

        free(block);
    }

    void Platform::MemoryClear(void* block, UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::MemoryClear' not supported for this platform")

        OTR_INTERNAL_ASSERT(block != nullptr)

        memset(block, 0, size);
    }

    void Platform::Log(const char* message, UInt8 level)
    {
        OTR_LOG_FATAL("'Platform::Log' not supported for this platform")

//        OTR_INTERNAL_ASSERT(level < 6)
    }

#else

    Platform* Platform::CreatePlatform()
    {
        OTR_LOG_FATAL("'Platform::CreatePlatform' not supported for this platform")

        return nullptr;
    }

    void Platform::DestroyPlatform(Platform* platform)
    {
        OTR_LOG_FATAL("'Platform::DestroyPlatform' not supported for this platform")
    }

    void* Platform::Allocate(UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::Allocate' not supported for this platform")

        return nullptr;
    }

    void* Platform::Reallocate(void* block, UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::Reallocate' not supported for this platform")

        return nullptr;
    }

    void Platform::Free(void* block)
    {
        OTR_LOG_FATAL("'Platform::Free' not supported for this platform")
    }

    void Platform::MemoryClear(void* block, UInt64 size)
    {
        OTR_LOG_FATAL("'Platform::MemoryClear' not supported for this platform")
    }

    void Platform::Log(const char* message, UInt8 level)
    {
        OTR_LOG_FATAL("'Platform::Log' not supported for this platform")
    }

#endif
}