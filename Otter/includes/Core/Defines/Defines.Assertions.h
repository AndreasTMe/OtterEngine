#ifndef OTTERENGINE_DEFINES_ASSERTIONS_H
#define OTTERENGINE_DEFINES_ASSERTIONS_H

#include "Defines.Platform.h"
#include "Defines.Compilation.h"

#if defined(OTR_DEBUG)
    #define OTR_INTERNAL_ASSERTIONS_ENABLED 1
    #define OTR_ASSERTIONS_ENABLED 1
#elif defined(OTR_EDITOR)
    #define OTR_INTERNAL_ASSERTIONS_ENABLED 0
    #define OTR_ASSERTIONS_ENABLED 1
#elif defined(OTR_RUNTIME)
    #define OTR_INTERNAL_ASSERTIONS_ENABLED 0
    #define OTR_ASSERTIONS_ENABLED 0
#endif

#if OTR_INTERNAL_ASSERTIONS_ENABLED
    #define OTR_INTERNAL_ASSERT(assertion)                              \
        {                                                               \
            if (assertion) { }                                          \
            else                                                        \
            {                                                           \
                Otter::Logger::GetBuilder()                             \
                    ->PrepareAssertion(#assertion, nullptr)             \
                    ->CaptureSource(__FILE__, __LINE__)                 \
                    ->Log();                                            \
                OTR_DEBUG_BREAK();                                      \
            }                                                           \
        }

    #define OTR_INTERNAL_ASSERT_MSG(assertion, ...)                     \
        {                                                               \
            if (assertion) { }                                          \
            else                                                        \
            {                                                           \
                Otter::Logger::GetBuilder()                             \
                    ->PrepareAssertion(#assertion, __VA_ARGS__)         \
                    ->CaptureSource(__FILE__, __LINE__)                 \
                    ->Log();                                            \
                OTR_DEBUG_BREAK();                                      \
            }                                                           \
        }

#else
    #define OTR_INTERNAL_ASSERT(assertion)
    #define OTR_INTERNAL_ASSERT_MSG(assertion, ...)
#endif

#if OTR_ASSERTIONS_ENABLED
    #define OTR_ASSERT_SCOPE(scope) { scope }
    #define OTR_ASSERT_VARIABLE_DECLARATION(declaration) declaration;
    #define OTR_ASSERT_METHOD_DECLARATION(declaration) declaration

    #define OTR_ASSERT(assertion)                                       \
        {                                                               \
            if (assertion) { }                                          \
            else                                                        \
            {                                                           \
                Otter::Logger::GetBuilder()                             \
                    ->PrepareAssertion(#assertion, nullptr)             \
                    ->CaptureSource(__FILE__, __LINE__)                 \
                    ->Log();                                            \
                OTR_DEBUG_BREAK();                                      \
            }                                                           \
        }

    #define OTR_ASSERT_MSG(assertion, ...)                              \
        {                                                               \
            if (assertion) { }                                          \
            else                                                        \
            {                                                           \
                Otter::Logger::GetBuilder()                             \
                    ->PrepareAssertion(#assertion, __VA_ARGS__)         \
                    ->CaptureSource(__FILE__, __LINE__)                 \
                    ->Log();                                            \
                OTR_DEBUG_BREAK();                                      \
            }                                                           \
        }
#else
    #define OTR_ASSERT_SCOPE(scope)
    #define OTR_ASSERT_VARIABLE_DECLARATION(declaration)
    #define OTR_ASSERT_METHOD_DECLARATION(declaration)

    #define OTR_ASSERT(assertion)
    #define OTR_ASSERT_MSG(assertion, ...)
#endif

#endif //OTTERENGINE_DEFINES_ASSERTIONS_H
