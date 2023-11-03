#include "Otter.PCH.h"

#include "Platform/Windows/Platform.Win32.h"

#if OTR_PLATFORM_WINDOWS

#include <windows.h>
#include <windowsx.h>

namespace Otter::Internal
{
    struct WindowsPlatformWindowData
    {
        HINSTANCE m_Instance;
        HWND      m_Window;
    };

    static LRESULT CALLBACK WindowProcedureCallbackOverride(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    Double64      s_ClockFrequency = 0.0;
    LARGE_INTEGER s_ClockStart     = { 0 };

#pragma clang diagnostic pop

    UnsafeHandle g_PlatformMemoryHandle;

    bool WindowsPlatform::Startup(const char* const title,
                                  UInt16 left,
                                  UInt16 top,
                                  UInt16 width,
                                  UInt16 height)
    {
        UInt64 platformContextSize    = OTR_ALIGNED_OFFSET(sizeof(PlatformContext), OTR_PLATFORM_MEMORY_ALIGNMENT);
        UInt64 platformWindowDataSize = OTR_ALIGNED_OFFSET(sizeof(WindowsPlatformWindowData),
                                                           OTR_PLATFORM_MEMORY_ALIGNMENT);

        g_PlatformMemoryHandle = Unsafe::New(platformContextSize + platformWindowDataSize);

        m_Context = (PlatformContext*) g_PlatformMemoryHandle.m_Pointer;
        m_Context->m_Window = (WindowsPlatformWindowData*) g_PlatformMemoryHandle.m_Pointer + platformContextSize;

        m_Width  = width;
        m_Height = height;

        RegisterEvents();

        if (!InitialiseWindow(title, left, top, width, height))
        {
            Unsafe::Delete(g_PlatformMemoryHandle);
            return false;
        }

        InitialiseInternalClock();
        m_IsRunning = true;

        return true;
    }

    void WindowsPlatform::Shutdown()
    {
        if (m_Context->m_Window)
        {
            DestroyWindow(((WindowsPlatformWindowData*) m_Context->m_Window)->m_Window);
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
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        return static_cast<Double64>(currentTime.QuadPart - s_ClockStart.QuadPart) * s_ClockFrequency;
    }

    const void* WindowsPlatform::GetUnsafeWindow() const
    {
        return ((WindowsPlatformWindowData*) m_Context->m_Window)->m_Window;
    }

    void WindowsPlatform::RegisterEvents()
    {
        GlobalActions::OnWindowClose += [&](const WindowCloseEvent& event)
        {
            m_IsRunning = false;

            return true;
        };
        GlobalActions::OnWindowResize += [&](const WindowResizeEvent& event)
        {
            m_Width  = event.GetWidth();
            m_Height = event.GetHeight();

            return true;
        };

        // TODO: Add functionality for the rest of the window events
    }

    bool WindowsPlatform::InitialiseWindow(const char* const title,
                                           UInt16 left,
                                           UInt16 top,
                                           UInt16 width,
                                           UInt16 height)
    {
        auto windowData = static_cast<WindowsPlatformWindowData*>(m_Context->m_Window);
        windowData->m_Instance = GetModuleHandleA(nullptr);

        const char* className = "WindowClass";

        WNDCLASSEX windowClass;
        windowClass.cbSize        = sizeof(WNDCLASSEX);
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hbrBackground = nullptr;
        windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hIcon         = LoadIcon(windowData->m_Instance, IDI_APPLICATION);
        windowClass.hIconSm       = LoadIcon(windowData->m_Instance, IDI_APPLICATION);
        windowClass.hInstance     = windowData->m_Instance;
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

        int clientLeft   = left;
        int clientTop    = top;
        int clientWidth  = width;
        int clientHeight = height;

        int windowLeft   = clientLeft;
        int windowTop    = clientTop;
        int windowWidth  = clientWidth;
        int windowHeight = clientHeight;

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
                                      title,
                                      windowStyle,
                                      windowLeft,
                                      windowTop,
                                      windowWidth,
                                      windowHeight,
                                      nullptr,
                                      nullptr,
                                      windowData->m_Instance,
                                      nullptr);

        if (!window)
        {
            MessageBoxA(nullptr, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
            OTR_LOG_FATAL("Failed to create window")

            return false;
        }
        else
        {
            windowData->m_Window = window;
        }

        // TODO: If the window should not accept input, use SW_SHOWNOACTIVATE instead of SW_SHOW
        // TODO: If the window should be initially minimized, use SW_SHOWMINIMIZED instead of SW_SHOW
        // TODO: If the window should be initially maximized, use SW_SHOWMAXIMIZED instead of SW_SHOW
        ShowWindow(window, SW_SHOW);

        return true;
    }

    void WindowsPlatform::InitialiseInternalClock()
    {
        LARGE_INTEGER clockFrequency;
        QueryPerformanceFrequency(&clockFrequency);
        s_ClockFrequency = 1.0 / static_cast<Double64>(clockFrequency.QuadPart);
        QueryPerformanceCounter(&s_ClockStart);
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
            case WM_PAINT:
            {
                // TODO: This will probably be removed in the future, once rendering is implemented
                PAINTSTRUCT ps;
                HDC         hdc = BeginPaint(window, &ps);
                FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(50, 50, 50)));
                EndPaint(window, &ps);
            }
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

                if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
                    EventSystem::Schedule<KeyPressedEvent>(keyCode);
                else
                    EventSystem::Schedule<KeyReleasedEvent>(keyCode);

                // TODO: Implement key hold event

                return 0;
            }
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
            case WM_LBUTTONUP:
            {
                if (message == WM_LBUTTONDOWN)
                    EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Left);
                else
                    EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Left);
            }
                break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            {
                if (message == WM_MBUTTONDOWN)
                    EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Middle);
                else
                    EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Middle);
            }
                break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            {
                if (message == WM_RBUTTONDOWN)
                    EventSystem::Schedule<MouseButtonPressedEvent>(MouseButton::Right);
                else
                    EventSystem::Schedule<MouseButtonReleasedEvent>(MouseButton::Right);
            }
                break;
            default:
                break;
        }

        return DefWindowProcA(window, message, wParam, lParam);
    }
}

OTR_WITH_TYPENAME(Otter::Internal::WindowsPlatformWindowData)

#endif // OTR_PLATFORM_WINDOWS