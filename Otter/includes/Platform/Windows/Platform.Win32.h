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
        explicit WindowsPlatform(const PlatformConfiguration& configuration)
            : Platform(), m_Title(configuration.Title),
              m_Width(configuration.Width), m_Height(configuration.Height),
              m_State(configuration.State), m_IsRunning(false)
        {
        }
        ~WindowsPlatform() final = default;

        bool TryInitialise() final;
        void Shutdown() final;
        void CaptureWindowEvents() final;

        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const final { return m_Width; }
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const final { return m_Height; }
        [[nodiscard]] OTR_INLINE bool IsRunning() const final { return m_IsRunning; }

        [[nodiscard]] Double64 GetAbsoluteTime() const final;

    private:
        const char* m_Title = nullptr;
        UInt16      m_Width     = 0;
        UInt16      m_Height    = 0;
        WindowState m_State     = WindowState::Default;
        bool        m_IsRunning = false;

        void RegisterEvents();
        bool TryInitialiseWindow();
        void InitialiseInternalClock();
    };
}

#endif // OTR_PLATFORM_WINDOWS

#endif //OTTERENGINE_PLATFORM_WIN32_H
