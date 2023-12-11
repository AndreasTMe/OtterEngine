#ifndef OTTERENGINE_PLATFORM_WIN32_H
#define OTTERENGINE_PLATFORM_WIN32_H

#include "Core/Defines/Defines.Platform.h"

#if OTR_PLATFORM_WINDOWS

#include "Platform/Platform.h"

namespace Otter::Internal
{
    /**
     * @brief Data structure used to store the Windows window and instance handles.
     */
    struct WindowsPlatformWindowData
    {
        HWND      WindowHandle;
        HINSTANCE InstanceHandle;
    };

    /**
     * @brief The Windows platform class
     *
     * @note Implements all the platform-specific functions from its base class and its role is to initialise a window,
     * capture events, handle memory and time, etc, using the Win32 API.
     */
    class WindowsPlatform final : public Platform
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param configuration The configuration of the platform's window.
         */
        explicit WindowsPlatform(const PlatformConfiguration& configuration)
            : Platform(), m_Title(configuration.Title),
              m_Width(configuration.Width), m_Height(configuration.Height),
              m_State(configuration.State), m_IsRunning(false)
        {
        }

        /**
         * @brief Destructor.
         */
        ~WindowsPlatform() final = default;

        /**
         * @brief Initialises the platform and creates a window.
         *
         * @return True if the platform was initialised successfully, false otherwise.
         */
        bool TryInitialise() final;

        /**
         * @brief Shuts down the platform and destroys the window.
         */
        void Shutdown() final;

        /**
         * @brief Captures all window events.
         */
        void CaptureWindowEvents() final;

        /**
         * @brief Checks if the platform is currently running.
         *
         * @return True if the platform is running, false otherwise.
         *
         * @note This function is used from the main loop to check if the platform is still running. While it is running,
         * the main loop will continue to run. When it returns false, the main loop will stop and the application will
         * eventually exit.
         */
        [[nodiscard]] OTR_INLINE bool IsRunning() const final { return m_IsRunning; }

        /**
         * @brief Getter for the window width.
         *
         * @return The width of the window.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const final { return m_Width; }

        /**
         * @brief Getter for the window height.
         *
         * @return The height of the window.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const final { return m_Height; }

        /**
         * @brief Getter for the platform absolute time.
         *
         * @return The absolute time of the platform.
         *
         * @note This function is used to calculate the delta time between frames.
         */
        [[nodiscard]] Double64 GetAbsoluteTime() const final;

    private:
        const char* m_Title = nullptr;
        UInt16      m_Width     = 0;
        UInt16      m_Height    = 0;
        WindowState m_State     = WindowState::Default;
        bool        m_IsRunning = false;

        /**
         * @brief Registers all window-related events (e.g. resize, close, etc.).
         */
        void RegisterEvents();

        /**
         * @brief Used to initialise the main window.
         *
         * @return True if the window was initialised successfully, false otherwise.
         */
        bool TryInitialiseWindow();

        /**
         * @brief Captures the internal clock frequency.
         *
         * @note Uses @code{.cpp}QueryPerformanceFrequency@endcode to capture the internal clock frequency.
         */
        static void CaptureInternalClockFrequency();
    };
}

#endif // OTR_PLATFORM_WINDOWS

#endif //OTTERENGINE_PLATFORM_WIN32_H
