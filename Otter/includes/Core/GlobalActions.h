#ifndef OTTERENGINE_GLOBALACTIONS_H
#define OTTERENGINE_GLOBALACTIONS_H

#include "Core/Defines.h"

#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    class GlobalActions final
    {
    public:
        static Func<bool, const Internal::WindowCloseEvent&>     OnWindowClose;
        static Func<bool, const Internal::WindowResizeEvent&>    OnWindowResize;
        static Func<bool, const Internal::WindowMinimizedEvent&> OnWindowMinimized;
        static Func<bool, const Internal::WindowMaximizedEvent&> OnWindowMaximized;
        static Func<bool, const Internal::WindowRestoredEvent&>  OnWindowRestored;
        static Func<bool, const Internal::WindowRefreshEvent&>   OnWindowRefresh;

        static Func<bool, const Internal::KeyPressedEvent&>  OnKeyPressed;
        static Func<bool, const Internal::KeyReleasedEvent&> OnKeyReleased;
        static Func<bool, const Internal::KeyHoldEvent&>     OnKeyHold;

        static Func<bool, const Internal::MouseButtonPressedEvent&>  OnMouseButtonPressed;
        static Func<bool, const Internal::MouseButtonReleasedEvent&> OnMouseButtonReleased;
        static Func<bool, const Internal::MouseScrollEvent&>         OnMouseScroll;
        static Func<bool, const Internal::MouseMovedEvent&>          OnMouseMoved;
        static Func<bool, const Internal::MouseDraggedEvent&>        OnMouseDragged;

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(GlobalActions)
    };
}

#endif //OTTERENGINE_GLOBALACTIONS_H

