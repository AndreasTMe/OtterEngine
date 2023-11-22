#ifndef OTTERENGINE_PLATFORM_H
#define OTTERENGINE_PLATFORM_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    struct PlatformContext
    {
        void* Data;
    };

    class Platform
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Platform);

        virtual bool Startup(const char* title, UInt16 width, UInt16 height) = 0;
        virtual void Shutdown() = 0;
        virtual void CaptureWindowEvents() = 0;

        [[nodiscard]] virtual bool IsRunning() const = 0;
        [[nodiscard]] virtual UInt16 GetWidth() const = 0;
        [[nodiscard]] virtual UInt16 GetHeight() const = 0;
        [[nodiscard]] virtual Double64 GetAbsoluteTime() const = 0;

        [[nodiscard]] const void* GetUnsafeContext() const { return m_Context; }

        [[nodiscard]] static Platform* CreatePlatform();
        static void DestroyPlatform(Platform* platform);

        static void* Allocate(UInt64 size);
        static void* Reallocate(void* block, UInt64 size);
        static void Free(void* block);
        static void MemoryClear(void* block, UInt64 size);

        static void Log(const char* message, UInt8 level);

    protected:
        PlatformContext* m_Context{ };
    };
}

#endif //OTTERENGINE_PLATFORM_H
