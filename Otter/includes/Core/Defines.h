#ifndef OTTERENGINE_DEFINES_H
#define OTTERENGINE_DEFINES_H

// HELP: Platform detection.

// Found this on Stack Overflow.
// Question: How to detect reliably Mac OS X, iOS, Linux, Windows in C preprocessor?
// Answer: https://stackoverflow.com/a/5920028

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #if defined(_WIN64)
        #define OTR_PLATFORM_WINDOWS 1
    #else
        #error "Only x64 Windows builds are supported!"
    #endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define OTR_PLATFORM_IOS 1
        #error "iOS is currently not supported!"
    #elif TARGET_OS_MAC
        #define OTR_PLATFORM_MACOS 1
        #error "MacOS is currently not supported!"
    #else
        #error "Not supported Apple platform!"
    #endif
#elif defined(__ANDROID__)
    #define OTR_PLATFORM_ANDROID 1
    #error "Android is currently not supported!"
#elif defined(__linux__)
    #define OTR_PLATFORM_LINUX 1
    #error "Linux is currently not supported!"
#else
    #error "Unsupported platform!"
#endif

#ifndef OTR_PLATFORM_WINDOWS
    #define OTR_PLATFORM_WINDOWS 0
#endif
#ifndef OTR_PLATFORM_IOS
    #define OTR_PLATFORM_IOS 0
#endif
#ifndef OTR_PLATFORM_MACOS
    #define OTR_PLATFORM_MACOS 0
#endif
#ifndef OTR_PLATFORM_ANDROID
    #define OTR_PLATFORM_ANDROID 0
#endif
#ifndef OTR_PLATFORM_LINUX
    #define OTR_PLATFORM_LINUX 0
#endif

#if OTR_PLATFORM_WINDOWS
    #include <Windows.h>

    #define OTR_MAIN() WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
    #define OTR_MAIN() main(int argc, char** argv)
#endif

#define OTR_PLATFORM_MEMORY_ALIGNMENT 8

// HELP: Compiler detection.

#if defined(__GNUC__)

    #define OTR_DLLEXPORT __attribute__ ((__visibility__ ("default")))
    #define OTR_DLLIMPORT
    #define OTR_INLINE inline
    #define OTR_DEBUG_BREAK() __builtin_trap()

#elif defined(__clang__)

#define OTR_DLLEXPORT __attribute__ ((__visibility__ ("default")))
    #define OTR_DLLIMPORT
    #define OTR_INLINE inline
    #define OTR_DEBUG_BREAK() __builtin_trap()

#elif defined(_MSC_VER)

    #if _MSC_VER < 1900
        #error "Visual Studio 2015 or newer is required."
    #endif

    #define OTR_DLLEXPORT __declspec(dllexport)
    #define OTR_DLLIMPORT __declspec(dllimport)
    #define OTR_INLINE __forceinline
    #define OTR_DEBUG_BREAK() __debugbreak()

#else

    #error "Unsupported compiler."

#endif

// HELP: Utility macros.

#define OTR_USING_BASE(BaseType) using base = BaseType;

#define OTR_NAME_OF(...) #__VA_ARGS__

#define OTR_BITMASK(value) (1 << value)
#define OTR_IS_POWER_OF_TWO(value) (value > 1 && !(value & (value - 1)))

#define OTR_ALIGNED_OFFSET(value, alignment) ((value + (alignment - 1)) & ~(alignment - 1))
#define OTR_ALIGNED_PADDING(address, alignment) ((alignment - (address % alignment)) % alignment)

#if OTR_DEBUG
    #define OTR_INTERNAL_DEBUG_BLOCK(code)  \
        {                                   \
            code                            \
        }
#elif OTR_EDITOR
#define OTR_INTERNAL_DEBUG_BLOCK(code)
#else
    #define OTR_INTERNAL_DEBUG_BLOCK(code)
#endif

#if OTR_DEBUG
    #define OTR_DEBUG_BLOCK(code)   \
        {                           \
            code                    \
        }
#elif OTR_EDITOR
#define OTR_DEBUG_BLOCK(code)   \
        {                           \
            code                    \
        }
#else
    #define OTR_DEBUG_BLOCK(code)
#endif

#if OTR_INTERNALS_TEST_VISIBLE
    #define OTR_TEST_VISIBLE public
#else
    #define OTR_TEST_VISIBLE private
#endif

#endif //OTTERENGINE_DEFINES_H