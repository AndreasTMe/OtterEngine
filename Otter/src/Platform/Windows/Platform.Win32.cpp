#include "Platform/Windows/Platform.Win32.h"

#if OTR_PLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>

#include "Core/Assert.h"
#include "Core/Memory.h"
#include "Core/Events/EventSystem.h"
#include "Core/GlobalActions.h"

namespace Otter::Internal
{
    static LRESULT CALLBACK WindowProcedureCallbackOverride(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    static void OnKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam);
    static void OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);

    static Double64 gs_ClockFrequency = 0.0;

    UnsafeHandle g_PlatformMemoryHandle;

    bool WindowsPlatform::TryInitialise()
    {
        constexpr UInt64 platformContextSize    = OTR_ALIGNED_OFFSET(sizeof(PlatformContext),
                                                                     OTR_PLATFORM_MEMORY_ALIGNMENT);
        constexpr UInt64 platformWindowDataSize = OTR_ALIGNED_OFFSET(sizeof(WindowsPlatformWindowData),
                                                                     OTR_PLATFORM_MEMORY_ALIGNMENT);

        g_PlatformMemoryHandle = Unsafe::New(platformContextSize + platformWindowDataSize);

        m_Context = (PlatformContext*) g_PlatformMemoryHandle.Pointer;
        m_Context->Data = (WindowsPlatformWindowData*) ((UIntPtr*) g_PlatformMemoryHandle.Pointer + 1);

        RegisterEvents();

        if (!TryInitialiseWindow())
        {
            Unsafe::Delete(g_PlatformMemoryHandle);
            return false;
        }

        CaptureInternalClockFrequency();
        m_IsRunning = true;

        return true;
    }

    void WindowsPlatform::Shutdown()
    {
        if (m_Context->Data)
        {
            DestroyWindow(((WindowsPlatformWindowData*) m_Context->Data)->WindowHandle);
            Unsafe::Delete(g_PlatformMemoryHandle);

            return;
        }

        OTR_LOG_WARNING(
            "Cannot destroy window because it might not have been created yet or it has already been destroyed")
    }

    void WindowsPlatform::CaptureWindowEvents()
    {
        MSG message;
        while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }

    Double64 WindowsPlatform::GetAbsoluteTime() const
    {
        LARGE_INTEGER performanceCounter;
        QueryPerformanceCounter(&performanceCounter);

        return static_cast<Double64>(performanceCounter.QuadPart) * gs_ClockFrequency;
    }

    void WindowsPlatform::RegisterEvents()
    {
        OTR_GLOBAL_ACTIONS.OnWindowClose >= [&](const WindowCloseEvent& event)
        {
            m_IsRunning = false;

            return true;
        };
        OTR_GLOBAL_ACTIONS.OnWindowResize >= [&](const WindowResizeEvent& event)
        {
            m_Width  = event.GetWidth();
            m_Height = event.GetHeight();

            // HELP: Pass the new width and height to the renderer
            return false;
        };
    }

    bool WindowsPlatform::TryInitialiseWindow()
    {
        auto windowData = static_cast<WindowsPlatformWindowData*>(m_Context->Data);
        windowData->InstanceHandle = GetModuleHandleA(nullptr);

        const char* className = "WindowClass";

        WNDCLASSEX windowClass;
        windowClass.cbSize        = sizeof(WNDCLASSEX);
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hbrBackground = nullptr;
        windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hIcon         = LoadIcon(windowData->InstanceHandle, IDI_APPLICATION);
        windowClass.hIconSm       = LoadIcon(windowData->InstanceHandle, IDI_APPLICATION);
        windowClass.hInstance     = windowData->InstanceHandle;
        windowClass.lpfnWndProc   = WindowProcedureCallbackOverride;
        windowClass.lpszClassName = className;
        windowClass.lpszMenuName  = nullptr;
        windowClass.style         = CS_DBLCLKS;

        if (!RegisterClassExA(&windowClass))
        {
            MessageBoxA(nullptr, "Failed to register window class", "Error", MB_OK | MB_ICONERROR);
            OTR_LOG_FATAL("Failed to register window class")

            return false;
        }

        RECT rect;
        GetClientRect(GetDesktopWindow(), &rect);

        int windowLeft   = (rect.right / 2) - (m_Width / 2);
        int windowTop    = (rect.bottom / 2) - (m_Height / 2);
        int windowWidth  = m_Width;
        int windowHeight = m_Height;

        int windowStyle   = WS_OVERLAPPEDWINDOW;
        int windowExStyle = WS_EX_APPWINDOW;

        RECT windowRect = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExStyle);

        windowLeft += windowRect.left;
        windowTop += windowRect.top;
        windowWidth += windowRect.right - windowRect.left;
        windowHeight += windowRect.bottom - windowRect.top;

        HWND window = CreateWindowExA(windowExStyle,
                                      className,
                                      m_Title,
                                      windowStyle,
                                      windowLeft,
                                      windowTop,
                                      windowWidth,
                                      windowHeight,
                                      nullptr,
                                      nullptr,
                                      windowData->InstanceHandle,
                                      nullptr);

        if (!window)
        {
            MessageBoxA(nullptr, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
            OTR_LOG_FATAL("Failed to create window")

            return false;
        }
        else
        {
            windowData->WindowHandle = window;
        }

        OTR_INTERNAL_ASSERT_MSG(m_State != WindowState::None && m_State != WindowState::Max,
                                "Window state cannot be None or Max")
        ShowWindow(window, m_State == WindowState::Maximised
                           ? SW_SHOWMAXIMIZED
                           : m_State == WindowState::Minimised
                             ? SW_SHOWMINIMIZED
                             : SW_SHOW);

        return true;
    }

    void WindowsPlatform::CaptureInternalClockFrequency()
    {
        LARGE_INTEGER clockFrequency;
        QueryPerformanceFrequency(&clockFrequency);
        gs_ClockFrequency = 1.0 / static_cast<Double64>(clockFrequency.QuadPart);
    }

    LRESULT CALLBACK WindowProcedureCallbackOverride(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        static bool userIsResizing = false;

        switch (message)
        {
            case WM_QUIT:
            case WM_CLOSE:
                EventSystem::Schedule<WindowCloseEvent>();
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_ERASEBKGND:
                // HELP: This is to prevent flickering when resizing the window
                return 1;
            case WM_SIZING:
                userIsResizing = true;
                break;
            case WM_EXITSIZEMOVE:
                userIsResizing = false;
                break;
            case WM_SIZE:
            {
                switch (wParam)
                {
                    case SIZE_MINIMIZED:
                        EventSystem::Schedule<WindowMinimizedEvent>();
                        break;
                    case SIZE_MAXIMIZED:
                        EventSystem::Schedule<WindowMaximizedEvent>();
                        break;
                    case SIZE_RESTORED:
                        EventSystem::Schedule<WindowRestoredEvent>();
                        break;
                    default:
                        break;
                }

                RECT clientRect;
                GetClientRect(window, &clientRect);
                EventSystem::Schedule<WindowResizeEvent>(clientRect.right - clientRect.left,
                                                         clientRect.bottom - clientRect.top,
                                                         userIsResizing);
            }
                break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
                OnKeyboardMessage(message, wParam, lParam);
                return 0;
            case WM_MOUSEMOVE:
            case WM_MOUSEWHEEL:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                OnMouseMessage(message, wParam, lParam);
                break;
            default:
                break;
        }

        return DefWindowProcA(window, message, wParam, lParam);
    }

    void OnKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        KeyCode keyCode    = KeyCode::None;
        bool    isExtended = (HIWORD(lParam) & KF_EXTENDED) == KF_EXTENDED;

        if (wParam == VK_MENU)
            keyCode = isExtended ? KeyCode::RightAlt : KeyCode::LeftAlt;
        else if (wParam == VK_SHIFT)
        {
            UInt32 leftShift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
            UInt32 scanCode  = ((lParam & (0xFF << 16)) >> 16);
            keyCode = scanCode == leftShift ? KeyCode::LeftShift : KeyCode::RightShift;
        }
        else if (wParam == VK_CONTROL)
            keyCode = isExtended ? KeyCode::RightCtrl : KeyCode::LeftCtrl;
        else
            keyCode = static_cast<KeyCode>(wParam);

        static UInt16 staticRepeatCount = 0;

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
        {

            const auto repeatCount = static_cast<UInt16>((lParam & 0xFFFF));
            if (repeatCount > 0)
            {
                staticRepeatCount += repeatCount;
                EventSystem::Schedule<KeyRepeatEvent>(keyCode, staticRepeatCount);
            }
            else
            {
                staticRepeatCount = 1;
                EventSystem::Schedule<KeyPressedEvent>(keyCode);
            }
        }
        else
        {
            staticRepeatCount = 0;
            EventSystem::Schedule<KeyReleasedEvent>(keyCode);
        }
    }

    void OnMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_MOUSEMOVE:
                EventSystem::Schedule<MouseMovedEvent>(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                break;
            case WM_MOUSEWHEEL:
            {
                Int16 delta = GET_WHEEL_DELTA_WPARAM(wParam);
                if (delta != 0)
                    EventSystem::Schedule<MouseScrollEvent>(delta > 0);
            }
                break;
            case WM_LBUTTONDOWN:
                EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Left);
                break;
            case WM_LBUTTONUP:
                EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Left);
                break;
            case WM_MBUTTONDOWN:
                EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Middle);
                break;
            case WM_MBUTTONUP:
                EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Middle);
                break;
            case WM_RBUTTONDOWN:
                EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Right);
                break;
            case WM_RBUTTONUP:
                EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Right);
                break;
            default:
                break;
        }
    }
}

#endif // OTR_PLATFORM_WINDOWS