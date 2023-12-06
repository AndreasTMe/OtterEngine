#ifndef OTTERENGINE_PLATFORM_H
#define OTTERENGINE_PLATFORM_H

#include "Core/Defines.h"
#include "Core/Types.h"

#define WINDOW_STATE_LIST           \
    REPLACE_WITH(Default, 0x01)     \
    REPLACE_WITH(Maximised, 0x02)   \
    REPLACE_WITH(Minimised, 0x03)

namespace Otter
{
    enum class WindowState : UInt8
    {
        None = 0x00,

#define REPLACE_WITH(Item, Value) Item = Value,
        WINDOW_STATE_LIST
#undef REPLACE_WITH

        Max = 0xFF
    };

    struct PlatformConfiguration final
    {
        const char* Title = nullptr;
        UInt16      Width  = 0;
        UInt16      Height = 0;
        WindowState State  = WindowState::Default;
    };

    struct PlatformContext
    {
        void* Data;
    };

    class Platform
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Platform);

        virtual bool TryInitialise() = 0;
        virtual void Shutdown() = 0;
        virtual void CaptureWindowEvents() = 0;

        [[nodiscard]] virtual bool IsRunning() const = 0;
        [[nodiscard]] virtual UInt16 GetWidth() const = 0;
        [[nodiscard]] virtual UInt16 GetHeight() const = 0;
        [[nodiscard]] virtual Double64 GetAbsoluteTime() const = 0;

        [[nodiscard]] const void* GetUnsafeContext() const { return m_Context; }

        [[nodiscard]] static Platform* CreatePlatform(const PlatformConfiguration& configuration);
        static void DestroyPlatform(Platform* platform);

        static void* Allocate(UInt64 size);
        static void* Reallocate(void* block, UInt64 size);
        static void Free(void* block);
        static void MemoryCopy(void* destination, const void* source, UInt64 size);
        static void MemoryClear(void* block, UInt64 size);
        static void SleepForMilliseconds(UInt64 value);

        static void Log(const char* message, UInt8 level);

    protected:
        PlatformContext* m_Context{ };
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::WindowState& windowState)
{
    switch (windowState)
    {
#define REPLACE_WITH(Item, Value) case Otter::WindowState::Item: os << "WindowState::" << #Item; break;
        WINDOW_STATE_LIST
#undef REPLACE_WITH
        default:
            os << "WindowState[Unknown]";
    }

    return os;
}
#undef WINDOW_STATE_LIST

#endif //OTTERENGINE_PLATFORM_H
