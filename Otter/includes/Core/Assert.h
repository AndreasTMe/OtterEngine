#ifndef OTTERENGINE_ASSERT_H
#define OTTERENGINE_ASSERT_H

#include "Core/Logger.h"

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

    #define OTR_INTERNAL_STATIC_ASSERT(assertion) static_assert(assertion, #assertion);

    #define OTR_INTERNAL_STATIC_ASSERT_MSG(assertion, message) static_assert(assertion, message);

#else
    #define OTR_INTERNAL_ASSERT(assertion)
    #define OTR_INTERNAL_ASSERT_MSG(assertion, ...)
    #define OTR_INTERNAL_STATIC_ASSERT(assertion)
    #define OTR_INTERNAL_STATIC_ASSERT_MSG(assertion, message)
#endif

#if OTR_ASSERTIONS_ENABLED
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

    #define OTR_STATIC_ASSERT(assertion) static_assert(assertion, #assertion);

    #define OTR_STATIC_ASSERT_MSG(assertion, message) static_assert(assertion, message);

#else
    #define OTR_ASSERT(assertion)
    #define OTR_ASSERT_MSG(assertion, ...)
    #define OTR_STATIC_ASSERT(assertion)
    #define OTR_STATIC_ASSERT_MSG(assertion, message)
#endif

#endif //OTTERENGINE_ASSERT_H
