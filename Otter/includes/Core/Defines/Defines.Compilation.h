#ifndef OTTERENGINE_DEFINES_COMPILATION_H
#define OTTERENGINE_DEFINES_COMPILATION_H

#if defined(__GNUC__)

#define OTR_DLLEXPORT __attribute__ ((__visibility__ ("default")))
#define OTR_DLLIMPORT
#define OTR_INLINE inline

#elif defined(__clang__)

#define OTR_DLLEXPORT __attribute__ ((__visibility__ ("default")))
#define OTR_DLLIMPORT
#define OTR_INLINE inline

#elif defined(_MSC_VER)

#if _MSC_VER < 1900
#error "Visual Studio 2015 or newer is required."
#endif

#define OTR_DLLEXPORT __declspec(dllexport)
#define OTR_DLLIMPORT __declspec(dllimport)
#define OTR_INLINE __forceinline

#else

#error "Unsupported compiler."

#endif

#endif //OTTERENGINE_DEFINES_COMPILATION_H
