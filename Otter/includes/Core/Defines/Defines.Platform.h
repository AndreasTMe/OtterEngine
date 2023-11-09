#ifndef OTTERENGINE_DEFINES_PLATFORM_H
#define OTTERENGINE_DEFINES_PLATFORM_H

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

#endif //OTTERENGINE_DEFINES_PLATFORM_H
