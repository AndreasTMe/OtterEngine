#ifndef OTTERENGINE_PLATFORM_WIN32_H
#define OTTERENGINE_PLATFORM_WIN32_H

#include "Core/Defines/Defines.Platform.h"

#if OTR_PLATFORM_WINDOWS

#include "Platform/Platform.h"

namespace Otter::Internal
{
    struct WindowsPlatformWindowData
    {
        HWND      WindowHandle;
        HINSTANCE InstanceHandle;
    };

    class WindowsPlatform final : public Platform
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_FINAL_DESTRUCTOR(WindowsPlatform);

        bool Startup(const char* title, UInt16 width, UInt16 height) final;
        void Shutdown() final;
        void CaptureWindowEvents() final;

        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const final { return m_Width; }
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const final { return m_Height; }
        [[nodiscard]] OTR_INLINE bool IsRunning() const final { return m_IsRunning; }

        [[nodiscard]] Double64 GetAbsoluteTime() const final;

    private:
        UInt16 m_Width     = 0;
        UInt16 m_Height    = 0;
        bool   m_IsRunning = false;

        void RegisterEvents();
        bool InitialiseWindow(const char* title,
                              UInt16 left,
                              UInt16 top,
                              UInt16 width,
                              UInt16 height);
        void InitialiseInternalClock();
    };
}

#endif // OTR_PLATFORM_WINDOWS

#endif //OTTERENGINE_PLATFORM_WIN32_H
